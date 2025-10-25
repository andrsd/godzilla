// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/RestartOutput.h"
#include "godzilla/CallStack.h"
#include "godzilla/Enums.h"
#include "godzilla/Parameters.h"
#include "godzilla/Problem.h"
#include "godzilla/RestartFile.h"
#include "godzilla/RestartInterface.h"

namespace godzilla {

Parameters
RestartOutput::parameters()
{
    auto params = FileOutput::parameters();
    params.set<ExecuteOn>("on", EXECUTE_ON_FINAL);
    return params;
}

RestartOutput::RestartOutput(const Parameters & pars) :
    FileOutput(pars),
    ri(dynamic_cast<RestartInterface *>(get_problem()))
{
}

void
RestartOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();
    if (ri == nullptr)
        log_warning("RestartOutput works only with problems that support restart.");
}

void
RestartOutput::output_step()
{
    CALL_STACK_MSG();
    RestartFile file(get_file_name(), FileAccess::CREATE);
    this->ri->write_restart_file(file);
}

std::string
RestartOutput::get_file_ext() const
{
    return { "restart.h5" };
}

} // namespace godzilla
