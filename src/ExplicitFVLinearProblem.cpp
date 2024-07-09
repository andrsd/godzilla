// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExplicitFVLinearProblem.h"
#include "godzilla/Output.h"

namespace godzilla {

Parameters
ExplicitFVLinearProblem::parameters()
{
    Parameters params = NonlinearProblem::parameters();
    params += ExplicitProblemInterface::parameters();
    return params;
}

ExplicitFVLinearProblem::ExplicitFVLinearProblem(const Parameters & params) :
    NonlinearProblem(params),
    FVProblemInterface(this, params),
    ExplicitProblemInterface(this, params)
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_INITIAL | EXECUTE_ON_TIMESTEP);
}

Real
ExplicitFVLinearProblem::get_time() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_time();
}

Int
ExplicitFVLinearProblem::get_step_num() const
{
    CALL_STACK_MSG();
    return ExplicitProblemInterface::get_step_number();
}

void
ExplicitFVLinearProblem::init()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::init();
    auto snes = ExplicitProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FVProblemInterface::init();
}

void
ExplicitFVLinearProblem::create()
{
    CALL_STACK_MSG();
    FVProblemInterface::create();
    NonlinearProblem::create();
    ExplicitProblemInterface::create();
}

void
ExplicitFVLinearProblem::run()
{
    CALL_STACK_MSG();
    set_up_initial_guess();
    on_initial();
    lprint(9, "Solving");
    TransientProblemInterface::solve(get_solution_vector());
    if (converged())
        on_final();
}

bool
ExplicitFVLinearProblem::converged()
{
    CALL_STACK_MSG();
    return TransientProblemInterface::get_converged_reason() > 0;
}

void
ExplicitFVLinearProblem::compute_solution_vector_local()
{
    CALL_STACK_MSG();
    auto loc_sln = get_solution_vector_local();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFVLinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    Problem::allocate_objects();
    FVProblemInterface::allocate_objects();
}

void
ExplicitFVLinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitFVLinearProblem::set_up_initial_guess()
{
    CALL_STACK_MSG();
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    DiscreteProblemInterface::set_up_initial_guess();
}

void
ExplicitFVLinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    NonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

void
ExplicitFVLinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexTSComputeRHSFunctionFVM(get_dm(), time, x, F, this));
}

void
ExplicitFVLinearProblem::compute_residual(const Vector & x, Vector & f)
{
}

void
ExplicitFVLinearProblem::compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp)
{
}

void
ExplicitFVLinearProblem::post_step()
{
    CALL_STACK_MSG();
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(EXECUTE_ON_TIMESTEP);
}

} // namespace godzilla
