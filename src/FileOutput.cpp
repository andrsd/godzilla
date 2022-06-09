#include "Godzilla.h"
#include "CallStack.h"
#include "FileOutput.h"
#include "Problem.h"
#include "App.h"
#include <boost/filesystem.hpp>

namespace godzilla {

InputParameters
FileOutput::valid_params()
{
    InputParameters params = Output::valid_params();
    params.add_param<std::string>("file", "", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const InputParameters & params) :
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
        const std::string & input_file_name = get_app()->get_input_file_name();
        this->file_base = boost::filesystem::path(input_file_name).stem().c_str();
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
    std::ostringstream oss;
    internal::fprintf(oss, "%s.%s", this->file_base, this->get_file_ext());
    this->file_name = oss.str();
}

void
FileOutput::set_sequence_file_name(unsigned int stepi)
{
    _F_;
    std::ostringstream oss;
    internal::fprintf(oss, "%s.%d.%s", this->file_base, stepi, this->get_file_ext());
    this->file_name = oss.str();
}

} // namespace godzilla
