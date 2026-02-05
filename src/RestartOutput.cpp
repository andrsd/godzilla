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
    params.set<ExecuteOnFlags>("on", ExecuteOn::FINAL);
    return params;
}

RestartOutput::RestartOutput(const Parameters & pars) :
    FileOutput(pars),
    ri(dynamic_ref_cast<RestartInterface>(get_problem())),
    file_base(pars.get<fs::path>("file"))
{
}

void
RestartOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();
    if (!this->ri.has_value())
        warning("RestartOutput works only with problems that support restart.");
}

void
RestartOutput::output_step()
{
    CALL_STACK_MSG();
    auto comm = get_comm();
    RestartFile file(comm, get_file_name(), FileAccess::CREATE);
    this->ri.value()->write_restart_file(file);
}

String
RestartOutput::get_file_ext() const
{
    return { "restart.h5" };
}

fs::path
RestartOutput::create_file_name() const
{
    return fmt::format("{}.{}", this->file_base.string(), this->get_file_ext());
}

} // namespace godzilla
