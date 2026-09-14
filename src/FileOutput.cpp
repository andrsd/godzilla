// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CallStack.h"
#include "godzilla/FileOutput.h"
#include "godzilla/Exception.h"
#include "godzilla/Problem.h"
#include "godzilla/App.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Ref.h"

namespace godzilla {

Parameters
FileOutput::parameters()
{
    auto params = Output::parameters();
    params.add_required_param<fs::path>("file", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const Parameters & pars) :
    Output(pars),
    file_base_(pars.get<fs::path>("file"))
{
    CALL_STACK_MSG();
    expect_true(!this->file_base_.empty(), "The 'file' parameter cannot be empty");
}

void
FileOutput::create()
{
    CALL_STACK_MSG();
    Output::create();
    this->file_name_ = create_file_name();
}

fs::path
FileOutput::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name_;
}

fs::path
FileOutput::create_file_name() const
{
    CALL_STACK_MSG();
    if (get_comm().size() == 1)
        return fmt::format("{}.{}", this->file_base_, this->get_file_ext());
    else
        return fmt::format("{}.{}.{}", this->file_base_, get_processor_id(), this->get_file_ext());
}

void
FileOutput::set_file_base(fs::path file_base)
{
    CALL_STACK_MSG();
    this->file_base_ = file_base;
}

fs::path
FileOutput::get_file_base() const
{
    CALL_STACK_MSG();
    return this->file_base_;
}

void
FileOutput::set_sequence_file_base(unsigned int stepi)
{
    CALL_STACK_MSG();
    this->file_base_ = fmt::format("{}.{}", this->file_base_, stepi);
    this->file_name_ = create_file_name();
}

} // namespace godzilla
