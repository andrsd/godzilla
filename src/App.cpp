// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Assert.h"

namespace godzilla {

App::App(mpi::Communicator comm, String name) : CoreApp(comm, name), problem_(nullptr)
{
    CALL_STACK_MSG();
}

App::App(mpi::Communicator comm, Registry & registry, String name) :
    CoreApp(comm, registry, name),
    problem_(nullptr)
{
    CALL_STACK_MSG();
}

Ref<Problem>
App::get_problem() const
{
    CALL_STACK_MSG();
    expect_true(this->problem_ != nullptr, "Problem is null");
    return ref(*this->problem_);
}

int
App::run()
{
    CALL_STACK_MSG();

    auto start_time = std::chrono::high_resolution_clock::now();
    run_problem();
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = end_time - start_time;
    write_perf_log(duration);

    return 0;
}

void
App::run_problem()
{
    CALL_STACK_MSG();
    expect_true(this->problem_ != nullptr, "Problem is null");
    this->problem_->create();

    lprintln(9, "Running");
    this->problem_->run();
}

} // namespace godzilla
