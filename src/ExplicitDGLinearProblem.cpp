// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExplicitDGLinearProblem.h"
#include "godzilla/Output.h"

namespace godzilla {

Parameters
ExplicitDGLinearProblem::parameters()
{
    Parameters params = NonlinearProblem::parameters();
    params += ExplicitProblemInterface::parameters();
    return params;
}

ExplicitDGLinearProblem::ExplicitDGLinearProblem(const Parameters & params) :
    NonlinearProblem(params),
    DGProblemInterface(this, params),
    ExplicitProblemInterface(this, params)
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_INITIAL | EXECUTE_ON_TIMESTEP);
}

ExplicitDGLinearProblem::~ExplicitDGLinearProblem()
{
}

Real
ExplicitDGLinearProblem::get_time() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_time();
}

Int
ExplicitDGLinearProblem::get_step_num() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_step_number();
}

void
ExplicitDGLinearProblem::init()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::init();
    auto snes = ExplicitProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    DGProblemInterface::init();
}

void
ExplicitDGLinearProblem::create()
{
    CALL_STACK_MSG();
    DGProblemInterface::create();
    NonlinearProblem::create();
    ExplicitProblemInterface::create();
}

bool
ExplicitDGLinearProblem::converged()
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::converged();
}

void
ExplicitDGLinearProblem::solve()
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    ExplicitProblemInterface::solve(get_solution_vector());
}

void
ExplicitDGLinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto loc_sln = get_solution_vector_local();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitDGLinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    Problem::allocate_objects();
    DGProblemInterface::allocate_objects();
}

void
ExplicitDGLinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitDGLinearProblem::set_up_initial_guess()
{
    CALL_STACK_MSG();
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    DiscreteProblemInterface::set_up_initial_guess();
}

void
ExplicitDGLinearProblem::set_up_time_scheme()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_time_scheme();
}

void
ExplicitDGLinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    NonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

void
ExplicitDGLinearProblem::post_step()
{
    CALL_STACK_MSG();
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(EXECUTE_ON_TIMESTEP);
}

} // namespace godzilla
