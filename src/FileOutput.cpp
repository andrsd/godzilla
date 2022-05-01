#include "Godzilla.h"
#include "FileOutput.h"

namespace godzilla {

static const int MAX_PATH = 1024;

InputParameters
FileOutput::valid_params()
{
    InputParameters params = Output::valid_params();
    params.add_required_param<std::string>("file", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const InputParameters & params) :
    Output(params),
    file_base(get_param<std::string>("file"))
{
    _F_;
}

FileOutput::~FileOutput()
{
    _F_;
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
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%s", this->file_base.c_str(), this->get_file_ext().c_str());
    this->file_name = std::string(fn);
}

} // namespace godzilla
