// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/FileOutput.h"
#include "godzilla/Exception.h"
#include "godzilla/Problem.h"
#include "godzilla/App.h"
#include "godzilla/DiscreteProblemInterface.h"
#include <filesystem>

namespace godzilla {

Parameters
FileOutput::parameters()
{
    auto params = Output::parameters();
    params.add_required_param<std::string>("file", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const Parameters & pars) :
    Output(pars),
    file_base(pars.get<std::string>("file")),
    dpi(dynamic_cast<DiscreteProblemInterface *>(get_problem()))
{
    CALL_STACK_MSG();
    if (this->file_base.empty())
        log_error("The 'file' parameter cannot be empty");
}

void
FileOutput::create()
{
    CALL_STACK_MSG();
    Output::create();
    this->file_name = create_file_name();
}

std::string
FileOutput::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name;
}

std::string
FileOutput::create_file_name() const
{
    CALL_STACK_MSG();
    if (get_comm().size() == 1)
        return fmt::format("{}.{}", this->file_base, this->get_file_ext());
    else
        return fmt::format("{}.{}.{}", this->file_base, get_processor_id(), this->get_file_ext());
}

void
FileOutput::set_file_base(const std::string & file_base)
{
    CALL_STACK_MSG();
    this->file_base = file_base;
}

const std::string &
FileOutput::get_file_base() const
{
    CALL_STACK_MSG();
    return this->file_base;
}

void
FileOutput::set_sequence_file_base(unsigned int stepi)
{
    CALL_STACK_MSG();
    this->file_base = fmt::format("{}.{}", this->file_base, stepi);
    this->file_name = create_file_name();
}

void
FileOutput::add_var_names(FieldID fid, std::vector<std::string> & var_names)
{
    CALL_STACK_MSG();
    const std::string & name = this->dpi->get_field_name(fid);
    Int nc = this->dpi->get_field_num_components(fid);
    if (nc == 1)
        var_names.push_back(name);
    else {
        for (Int c = 0; c < nc; ++c) {
            std::string comp_name = this->dpi->get_field_component_name(fid, c);
            std::string s;
            if (comp_name.empty())
                s = fmt::format("{}_{}", name, c);
            else
                s = fmt::format("{}", comp_name);
            var_names.push_back(s);
        }
    }
}

void
FileOutput::add_aux_var_names(FieldID fid, std::vector<std::string> & var_names)
{
    CALL_STACK_MSG();
    const std::string & name = this->dpi->get_aux_field_name(fid);
    Int nc = this->dpi->get_aux_field_num_components(fid);
    if (nc == 1)
        var_names.push_back(name);
    else {
        for (Int c = 0; c < nc; ++c) {
            std::string comp_name = this->dpi->get_aux_field_component_name(fid, c);
            std::string s;
            if (comp_name.empty())
                s = fmt::format("{}_{}", name, c);
            else
                s = fmt::format("{}", comp_name);
            var_names.push_back(s);
        }
    }
}

const DiscreteProblemInterface *
FileOutput::get_discrete_problem_interface() const
{
    CALL_STACK_MSG();
    return this->dpi;
}

DiscreteProblemInterface *
FileOutput::get_discrete_problem_interface()
{
    CALL_STACK_MSG();
    return this->dpi;
}

} // namespace godzilla
