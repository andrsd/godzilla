// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/FileOutput.h"
#include "godzilla/Problem.h"
#include "godzilla/App.h"
#include <filesystem>

namespace godzilla {

Parameters
FileOutput::parameters()
{
    Parameters params = Output::parameters();
    params.add_param<std::string>("file", "", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const Parameters & params) :
    Output(params),
    file_base(get_param<std::string>("file"))
{
    _F_;
}

void
FileOutput::create()
{
    _F_;
    Output::create();
    if (this->file_base.length() == 0) {
        std::filesystem::path input_file_name(get_app()->get_input_file_name());
        this->file_base = input_file_name.stem().u8string();
    }
}

std::string
FileOutput::get_file_name() const
{
    _F_;
    if (get_comm().size() == 1)
        return fmt::format("{}.{}", this->file_base, this->get_file_ext());
    else
        return fmt::format("{}.{}.{}", this->file_base, get_processor_id(), this->get_file_ext());
}

void
FileOutput::set_file_base(const std::string & file_base)
{
    _F_;
    this->file_base = file_base;
}

void
FileOutput::set_sequence_file_base(unsigned int stepi)
{
    _F_;
    this->file_base = fmt::format("{}.{}", this->file_base, stepi);
}

} // namespace godzilla
