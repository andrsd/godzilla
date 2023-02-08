#include "CSVOutput.h"
#include "Godzilla.h"
#include "CallStack.h"
#include "Problem.h"
#include "Postprocessor.h"
#include "Error.h"
#include "fmt/printf.h"
#include <cstring>
#include <cerrno>

namespace godzilla {

REGISTER_OBJECT(CSVOutput);

Parameters
CSVOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    return params;
}

CSVOutput::CSVOutput(const Parameters & params) : FileOutput(params), f(nullptr), has_header(false)
{
}

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

    if (!this->pps_names.empty())
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
    if (this->pps_names.empty())
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
    this->f = fopen(this->file_name.c_str(), "w");
    if (this->f == nullptr)
        log_error("Unable to open '%s' for writing: %s.", this->file_name, strerror(errno));
}

void
CSVOutput::write_header()
{
    _F_;
    fmt::fprintf(this->f, "time");
    for (auto & name : this->pps_names)
        fmt::fprintf(this->f, ",%s", name.c_str());
    fmt::fprintf(this->f, "\n");
}

void
CSVOutput::write_values(Real time)
{
    _F_;
    fmt::fprintf(this->f, "%g", time);
    for (auto & name : this->pps_names) {
        auto * pps = this->problem->get_postprocessor(name);
        Real val = pps->get_value();
        fmt::fprintf(this->f, ",%g", val);
    }
    fmt::fprintf(this->f, "\n");
}

void
CSVOutput::close_file()
{
    _F_;
    if (this->f != nullptr) {
        fclose(this->f);
        this->f = nullptr;
    }
}

} // namespace godzilla
