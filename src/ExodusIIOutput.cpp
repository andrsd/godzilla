// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/IO.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/DGProblemInterface.h"
#include "godzilla/Ref.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Exception.h"
#include "fmt/format.h"
#include "fmt/chrono.h"
#include <set>

namespace godzilla {

Parameters
ExodusIIOutput::parameters()
{
    auto params = FileOutput::parameters();
    params.add_param<bool>("append", false, "Append into an existing exodusII file");
    params.add_param<std::vector<String>>(
        "variables",
        std::vector<String> {},
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const Parameters & pars) :
    FileOutput(pars),
    dpi_(dynamic_ref_cast<DiscreteProblemInterface>(pars.get<Ref<Problem>>("_problem"))),
    mesh_(dpi_->get_mesh()),
    append_(pars.get<bool>("append")),
    variable_names_(pars.get<std::vector<String>>("variables"), {}),
    step_num_(1),
    mesh_stored_(false)
{
    CALL_STACK_MSG();
}

ExodusIIOutput::~ExodusIIOutput()
{
    CALL_STACK_MSG();
    if (this->exo_)
        this->exo_->close();
}

String
ExodusIIOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    return { "exo" };
}

void
ExodusIIOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();

    auto flds = this->dpi_->get_field_names();
    auto aux_flds = this->dpi_->get_aux_field_names();
    auto & pps = get_problem()->get_postprocessor_names();

    if (this->variable_names_.empty()) {
        this->field_var_names_ = flds;
        this->aux_field_var_names_ = aux_flds;
        for (auto & name : pps)
            this->global_var_names_.push_back(name);
    }
    else {
        std::set<String> field_names(flds.begin(), flds.end());
        std::set<String> aux_field_names(aux_flds.begin(), aux_flds.end());
        std::set<String> pp_names(pps.begin(), pps.end());

        for (auto & name : this->variable_names_) {
            if (field_names.count(name) == 1)
                this->field_var_names_.push_back(name);
            else if (aux_field_names.count(name) == 1)
                this->aux_field_var_names_.push_back(name);
            else if (pp_names.count(name) == 1)
                this->global_var_names_.push_back(name);
            else
                error("Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                      name);
        }
    }

    int n_nodal_var_names = 1;
    int n_elem_var_names = 1;
    for (auto & name : this->field_var_names_) {
        auto fid = this->dpi_->get_field_id(name).value();
        auto order = this->dpi_->get_field_order(fid).value();
        auto nc = this->dpi_->get_field_num_components(fid).value();
        if (order == 0) {
            this->elem_var_fids_.push_back({ fid, n_elem_var_names });
            n_elem_var_names += nc;
        }
        else {
            this->nodal_var_fids_.push_back({ fid, n_nodal_var_names });
            n_nodal_var_names += nc;
        }
    }
    for (auto & name : this->aux_field_var_names_) {
        auto fid = this->dpi_->get_aux_field_id(name).value();
        auto order = this->dpi_->get_aux_field_order(fid).value();
        auto nc = this->dpi_->get_aux_field_num_components(fid).value();
        if (order == 0) {
            this->elem_aux_var_fids_.push_back({ fid, n_elem_var_names });
            n_elem_var_names += nc;
        }
        else {
            this->nodal_aux_var_fids_.push_back({ fid, n_nodal_var_names });
            n_nodal_var_names += nc;
        }
    }
}

void
ExodusIIOutput::output_mesh()
{
    CALL_STACK_MSG();
    if (this->exo_ == nullptr)
        open_file();

    if (!this->mesh_stored_) {
        write_info();
        // TODO: should we you call `write_mesh` for DG problems?
        io::write_mesh(*this->exo_, *this->mesh_);
    }
}
void
ExodusIIOutput::output_step()
{
    CALL_STACK_MSG();
    if (auto dgpi = try_dynamic_ref_cast<const DGProblemInterface>(get_problem());
        dgpi.has_value()) {
        output_step(*dgpi.value());
    }
    else {
        output_step(*this->dpi_);
    }
}

void
ExodusIIOutput::output_step(const DiscreteProblemInterface & iface)
{
    CALL_STACK_MSG();
    if (this->exo_ == nullptr)
        open_file();

    if (!this->mesh_stored_) {
        this->mesh_stored_ = true;
        write_info();
        io::write_mesh(*this->exo_, *this->mesh_);
        write_all_variable_names();
    }

    Real time = get_problem()->get_time();
    this->exo_->write_time(this->step_num_, time);

    for (auto [fid, exo_var_id] : this->nodal_var_fids_) {
        io::write_field_values(*this->exo_, iface, this->step_num_, time, fid, exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->nodal_aux_var_fids_) {
        io::write_aux_field_values(*this->exo_, iface, this->step_num_, time, fid, exo_var_id);
    }

    write_elem_variables();
    write_global_variables();

    this->exo_->update();

    ++this->step_num_;
}

void
ExodusIIOutput::output_step(const DGProblemInterface & dgpi)
{
    CALL_STACK_MSG();
    if (this->exo_ == nullptr)
        open_file();

    if (!this->mesh_stored_) {
        this->mesh_stored_ = true;
        write_info();
        io::write_mesh_discontinuous(*this->exo_, *this->mesh_);
        write_all_variable_names();
    }

    Real time = get_problem()->get_time();
    this->exo_->write_time(this->step_num_, time);

    for (auto [fid, exo_var_id] : this->nodal_var_fids_) {
        io::write_field_values(*this->exo_, dgpi, this->step_num_, time, fid, exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->nodal_aux_var_fids_) {
        io::write_aux_field_values(*this->exo_, dgpi, this->step_num_, time, fid, exo_var_id);
    }

    write_elem_variables();
    write_global_variables();

    this->exo_->update();

    ++this->step_num_;
}

void
ExodusIIOutput::open_file()
{
    CALL_STACK_MSG();
    // clang-format off
    auto file_access = this->append_ ?
        exodusIIcpp::FileAccess::APPEND :
        exodusIIcpp::FileAccess::WRITE;
    // clang-format on

    auto file_name = get_file_name();
    if (not fs::exists(file_name))
        file_access = exodusIIcpp::FileAccess::WRITE;

    this->exo_ = Qtr<exodusIIcpp::File>::alloc(file_name, file_access);
    if (!this->exo_->is_opened())
        throw Exception(fmt::format("Could not open file '{}' for writing.", file_name));

    if (file_access == exodusIIcpp::FileAccess::APPEND) {
        this->exo_->read_times();
        this->step_num_ = this->exo_->get_num_times() + 1;
        this->mesh_stored_ = true;
    }
}

void
ExodusIIOutput::write_all_variable_names()
{
    CALL_STACK_MSG();

    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names_) {
        auto fid = this->dpi_->get_field_id(name).value();
        auto order = this->dpi_->get_field_order(fid).value();
        if (order == 0) {
            auto names = io::get_var_names(*this->dpi_, fid);
            elem_var_names.insert(elem_var_names.end(), names.begin(), names.end());
        }
        else {
            auto names = io::get_var_names(*this->dpi_, fid);
            nodal_var_names.insert(nodal_var_names.end(), names.begin(), names.end());
        }
    }
    for (auto & name : this->aux_field_var_names_) {
        auto fid = this->dpi_->get_aux_field_id(name).value();
        auto order = this->dpi_->get_aux_field_order(fid).value();
        if (order == 0) {
            auto names = io::get_aux_var_names(*this->dpi_, fid);
            elem_var_names.insert(elem_var_names.end(), names.begin(), names.end());
        }
        else {
            auto names = io::get_aux_var_names(*this->dpi_, fid);
            nodal_var_names.insert(nodal_var_names.end(), names.begin(), names.end());
        }
    }
    this->exo_->write_nodal_var_names(nodal_var_names);
    this->exo_->write_elem_var_names(elem_var_names);
    this->exo_->write_global_var_names(this->global_var_names_);
}

void
ExodusIIOutput::write_elem_variables()
{
    CALL_STACK_MSG();

    Real time = get_problem()->get_time();
    this->exo_->write_time(this->step_num_, time);

    for (auto [fid, exo_var_id] : this->elem_var_fids_) {
        io::write_elemental_field_values(*this->exo_,
                                         *this->dpi_,
                                         this->step_num_,
                                         time,
                                         fid,
                                         exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->elem_aux_var_fids_) {
        io::write_aux_elemental_field_values(*this->exo_,
                                             *this->dpi_,
                                             this->step_num_,
                                             time,
                                             fid,
                                             exo_var_id);
    }
}

void
ExodusIIOutput::write_global_variables()
{
    CALL_STACK_MSG();

    int exo_var_id = 1;
    for (auto & name : this->global_var_names_) {
        auto pp = get_problem()->get_postprocessor(name).value();
        auto vals = pp->get_value();
        // FIXME: store all components
        this->exo_->write_global_var(this->step_num_, exo_var_id, vals[0]);
        ++exo_var_id;
    }
}

void
ExodusIIOutput::write_info()
{
    CALL_STACK_MSG();
    auto app = get_app();
    std::time_t now = std::time(nullptr);
    String datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));
    String created_by =
        fmt::format("Created by {} {}, on {}", app->get_name(), app->get_version(), datetime);

    std::vector<std::string> info;
    info.push_back(created_by);
    this->exo_->write_info(info);
}

} // namespace godzilla
