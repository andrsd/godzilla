// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CSVOutput.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/Postprocessor.h"
#include "fmt/printf.h"
#include <cstring>
#include <cerrno>

namespace godzilla {

Parameters
CSVOutput::parameters()
{
    auto params = FileOutput::parameters();
    return params;
}

CSVOutput::CSVOutput(const Parameters & pars) : FileOutput(pars), f_(nullptr), has_header_(false) {}

CSVOutput::~CSVOutput()
{
    CALL_STACK_MSG();
    close_file();
}

void
CSVOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();

    this->pps_names_ = get_problem()->get_postprocessor_names();

    if (!this->pps_names_.empty())
        open_file();
}

String
CSVOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    return "csv";
}

void
CSVOutput::output_step()
{
    CALL_STACK_MSG();
    if (this->pps_names_.empty())
        return;

    if (!this->has_header_) {
        write_header();
        this->has_header_ = true;
    }
    write_values(get_problem()->get_time());
}

void
CSVOutput::open_file()
{
    CALL_STACK_MSG();
    this->f_ = fopen(get_file_name().c_str(), "w");
    expect_true(
        this->f_ != nullptr,
        fmt::format("Unable to open '{}' for writing: {}.", get_file_name(), strerror(errno)));
}

void
CSVOutput::write_header()
{
    CALL_STACK_MSG();
    fmt::print(this->f_, "time");
    for (auto & name : this->pps_names_)
        fmt::print(this->f_, ",{}", name);
    fmt::print(this->f_, "\n");
}

void
CSVOutput::write_values(Real time)
{
    CALL_STACK_MSG();
    fmt::print(this->f_, "{:g}", time);
    for (auto & name : this->pps_names_) {
        auto pps = get_problem()->get_postprocessor(name).value();
        auto vals = pps->get_value();
        // FIXME: store all components
        fmt::print(this->f_, ",{:g}", vals[0]);
    }
    fmt::print(this->f_, "\n");
}

void
CSVOutput::close_file()
{
    CALL_STACK_MSG();
    if (this->f_ != nullptr) {
        fclose(this->f_);
        this->f_ = nullptr;
    }
}

} // namespace godzilla
