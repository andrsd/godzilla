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
    params.add_param<std::vector<String>>(
        "variables",
        std::vector<String> {},
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const Parameters & pars) :
    FileOutput(pars),
    DiscreteProblemOutputInterface(pars),
    mesh(get_mesh()),
    variable_names(pars.get<std::vector<String>>("variables"), {}),
    step_num(1),
    mesh_stored(false)
{
    CALL_STACK_MSG();
}

ExodusIIOutput::~ExodusIIOutput()
{
    CALL_STACK_MSG();
    if (this->exo)
        this->exo->close();
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

    auto dpi = get_discrete_problem_interface();
    auto flds = dpi->get_field_names();
    auto aux_flds = dpi->get_aux_field_names();
    auto & pps = get_problem()->get_postprocessor_names();

    if (this->variable_names.empty()) {
        this->field_var_names = flds;
        this->aux_field_var_names = aux_flds;
        for (auto & name : pps)
            this->global_var_names.push_back(name);
    }
    else {
        std::set<String> field_names(flds.begin(), flds.end());
        std::set<String> aux_field_names(aux_flds.begin(), aux_flds.end());
        std::set<String> pp_names(pps.begin(), pps.end());

        for (auto & name : this->variable_names) {
            if (field_names.count(name) == 1)
                this->field_var_names.push_back(name);
            else if (aux_field_names.count(name) == 1)
                this->aux_field_var_names.push_back(name);
            else if (pp_names.count(name) == 1)
                this->global_var_names.push_back(name);
            else
                error("Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                      name);
        }
    }
}

void
ExodusIIOutput::output_mesh()
{
    CALL_STACK_MSG();
    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        write_info();
        // TODO: should we you call `write_mesh` for DG problems?
        io::write_mesh(*this->exo, *this->mesh);
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
        output_step(*get_discrete_problem_interface());
    }
}

void
ExodusIIOutput::output_step(const DiscreteProblemInterface & dpi)
{
    CALL_STACK_MSG();
    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        this->mesh_stored = true;
        write_info();
        io::write_mesh(*this->exo, *this->mesh);
        write_all_variable_names();
    }

    Real time = get_problem()->get_time();
    this->exo->write_time(this->step_num, time);

    for (auto [fid, exo_var_id] : this->nodal_var_fids) {
        io::write_field_values(*this->exo, dpi, this->step_num, time, fid, exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->nodal_aux_var_fids) {
        io::write_aux_field_values(*this->exo, dpi, this->step_num, time, fid, exo_var_id);
    }

    write_elem_variables();
    write_global_variables();

    this->exo->update();

    ++this->step_num;
}

void
ExodusIIOutput::output_step(const DGProblemInterface & dgpi)
{
    CALL_STACK_MSG();
    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        this->mesh_stored = true;
        write_info();
        io::write_mesh_discontinuous(*this->exo, *this->mesh);
        write_all_variable_names();
    }

    Real time = get_problem()->get_time();
    this->exo->write_time(this->step_num, time);

    for (auto [fid, exo_var_id] : this->nodal_var_fids) {
        io::write_field_values(*this->exo, dgpi, this->step_num, time, fid, exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->nodal_aux_var_fids) {
        io::write_aux_field_values(*this->exo, dgpi, this->step_num, time, fid, exo_var_id);
    }

    write_elem_variables();
    write_global_variables();

    this->exo->update();

    ++this->step_num;
}

void
ExodusIIOutput::open_file()
{
    CALL_STACK_MSG();
    this->exo =
        Qtr<exodusIIcpp::File>::alloc(get_file_name().c_str(), exodusIIcpp::FileAccess::WRITE);
    if (!this->exo->is_opened())
        throw Exception(fmt::format("Could not open file '{}' for writing.", get_file_name()));
}

void
ExodusIIOutput::write_all_variable_names()
{
    CALL_STACK_MSG();

    auto dpi = get_discrete_problem_interface();
    this->nodal_var_fids.clear();
    this->nodal_aux_var_fids.clear();
    this->elem_var_fids.clear();
    this->elem_aux_var_fids.clear();
    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names) {
        auto fid = dpi->get_field_id(name).value();
        auto order = dpi->get_field_order(fid).value();
        if (order == 0) {
            this->elem_var_fids.push_back({ fid, elem_var_names.size() + 1 });
            auto names = io::get_var_names(*dpi, fid);
            elem_var_names.insert(elem_var_names.end(), names.begin(), names.end());
        }
        else {
            this->nodal_var_fids.push_back({ fid, nodal_var_names.size() + 1 });
            auto names = io::get_var_names(*dpi, fid);
            nodal_var_names.insert(nodal_var_names.end(), names.begin(), names.end());
        }
    }
    for (auto & name : this->aux_field_var_names) {
        auto fid = dpi->get_aux_field_id(name).value();
        auto order = dpi->get_aux_field_order(fid).value();
        if (order == 0) {
            this->elem_aux_var_fids.push_back({ fid, elem_var_names.size() + 1 });
            auto names = io::get_aux_var_names(*dpi, fid);
            elem_var_names.insert(elem_var_names.end(), names.begin(), names.end());
        }
        else {
            this->nodal_aux_var_fids.push_back({ fid, nodal_var_names.size() + 1 });
            auto names = io::get_aux_var_names(*dpi, fid);
            nodal_var_names.insert(nodal_var_names.end(), names.begin(), names.end());
        }
    }
    this->exo->write_nodal_var_names(nodal_var_names);
    this->exo->write_elem_var_names(elem_var_names);
    this->exo->write_global_var_names(this->global_var_names);
}

void
ExodusIIOutput::write_elem_variables()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();

    Real time = get_problem()->get_time();
    this->exo->write_time(this->step_num, time);

    for (auto [fid, exo_var_id] : this->elem_var_fids) {
        io::write_elemental_field_values(*this->exo, *dpi, this->step_num, time, fid, exo_var_id);
    }
    for (auto [fid, exo_var_id] : this->elem_aux_var_fids) {
        io::write_aux_elemental_field_values(*this->exo,
                                             *dpi,
                                             this->step_num,
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
    for (auto & name : this->global_var_names) {
        auto pp = get_problem()->get_postprocessor(name).value();
        auto vals = pp->get_value();
        // FIXME: store all components
        this->exo->write_global_var(this->step_num, exo_var_id, vals[0]);
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
    this->exo->write_info(info);
}

} // namespace godzilla
