#include "CSVOutput.h"
#include "Godzilla.h"
#include "CallStack.h"
#include "Problem.h"
#include "Postprocessor.h"
#include "Error.h"

namespace godzilla {

registerObject(CSVOutput);

InputParameters
CSVOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

CSVOutput::CSVOutput(const InputParameters & params) : FileOutput(params), has_header(false) {}

CSVOutput::~CSVOutput()
{
    close_file();
}

void
CSVOutput::create()
{
    _F_;
    FileOutput::create();

    set_file_name();

    this->pps_names = this->problem->get_postprocessor_names();

    if (this->pps_names.size() > 0)
        open_file();
}

std::string
CSVOutput::get_file_ext() const
{
    _F_;
    return "csv";
}

void
CSVOutput::output_step()
{
    _F_;
    if (this->pps_names.size() == 0)
        return;

    lprintf(9, "Output to file: %s", this->file_name);

    if (!this->has_header) {
        write_header();
        this->has_header = true;
    }
    write_values(this->problem->get_time());
}

void
CSVOutput::open_file()
{
    _F_;
    this->f.open(this->file_name, std::ofstream::out);
    if (!this->f.is_open())
        log_error("Unable to open '%s' for writing.", this->file_name);
}

void
CSVOutput::write_header()
{
    _F_;
    internal::fprintf(this->f, "time");
    for (auto & name : this->pps_names)
        internal::fprintf(this->f, ",%s", name);
    internal::fprintf(this->f, "\n");
}

void
CSVOutput::write_values(PetscReal time)
{
    _F_;
    internal::fprintf(this->f, "%g", time);
    for (auto & name : this->pps_names) {
        auto * pps = this->problem->get_postprocessor(name);
        PetscReal val = pps->get_value();
        internal::fprintf(this->f, ",%g", val);
    }
    internal::fprintf(this->f, "\n");
}

void
CSVOutput::close_file()
{
    _F_;
    if (this->f.is_open())
        this->f.close();
}

} // namespace godzilla
