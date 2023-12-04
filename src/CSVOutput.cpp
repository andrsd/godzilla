// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CSVOutput.h"
#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/Postprocessor.h"
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
    _F_;
    close_file();
}

void
CSVOutput::create()
{
    _F_;
    FileOutput::create();

    this->pps_names = get_problem()->get_postprocessor_names();

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

    lprint(9, "Output to file: {}", get_file_name());

    if (!this->has_header) {
        write_header();
        this->has_header = true;
    }
    write_values(get_problem()->get_time());
}

void
CSVOutput::open_file()
{
    _F_;
    this->f = fopen(get_file_name().c_str(), "w");
    if (this->f == nullptr)
        log_error("Unable to open '{}' for writing: {}.", get_file_name(), strerror(errno));
}

void
CSVOutput::write_header()
{
    _F_;
    fmt::print(this->f, "time");
    for (auto & name : this->pps_names)
        fmt::print(this->f, ",{}", name);
    fmt::print(this->f, "\n");
}

void
CSVOutput::write_values(Real time)
{
    _F_;
    fmt::print(this->f, "{:g}", time);
    for (auto & name : this->pps_names) {
        auto * pps = get_problem()->get_postprocessor(name);
        Real val = pps->get_value();
        fmt::print(this->f, ",{:g}", val);
    }
    fmt::print(this->f, "\n");
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
