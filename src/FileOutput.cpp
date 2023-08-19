#include "Godzilla.h"
#include "CallStack.h"
#include "FileOutput.h"
#include "Problem.h"
#include "App.h"
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

const std::string &
FileOutput::get_file_name() const
{
    _F_;
    return this->file_name;
}

void
FileOutput::set_file_name()
{
    _F_;
    this->file_name = fmt::format("{}.{}", this->file_base, this->get_file_ext());
}

void
FileOutput::set_sequence_file_name(unsigned int stepi)
{
    _F_;
    this->file_name = fmt::format("{}.{}.{}", this->file_base, stepi, this->get_file_ext());
}

} // namespace godzilla
