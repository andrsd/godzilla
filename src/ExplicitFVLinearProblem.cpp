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
    _F_;
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
}

ExplicitFVLinearProblem::~ExplicitFVLinearProblem()
{
    set_snes(nullptr);
}

Real
ExplicitFVLinearProblem::get_time() const
{
    _F_;
    return ExplicitProblemInterface::get_time();
}

Int
ExplicitFVLinearProblem::get_step_num() const
{
    _F_;
    return ExplicitProblemInterface::get_step_number();
}

void
ExplicitFVLinearProblem::init()
{
    _F_;
    ExplicitProblemInterface::init();
    auto snes = ExplicitProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    FVProblemInterface::init();
}

void
ExplicitFVLinearProblem::create()
{
    _F_;
    FVProblemInterface::create();
    NonlinearProblem::create();
    ExplicitProblemInterface::create();
}

void
ExplicitFVLinearProblem::check()
{
    _F_;
    NonlinearProblem::check();
    ExplicitProblemInterface::check();
}

bool
ExplicitFVLinearProblem::converged()
{
    _F_;
    return ExplicitProblemInterface::converged();
}

void
ExplicitFVLinearProblem::solve()
{
    _F_;
    lprint(9, "Solving");
    ExplicitProblemInterface::solve(get_solution_vector());
}

void
ExplicitFVLinearProblem::compute_solution_vector_local()
{
    _F_;
    auto loc_sln = get_solution_vector_local();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitFVLinearProblem::allocate_objects()
{
    _F_;
    Problem::allocate_objects();
    FVProblemInterface::allocate_objects();
}

void
ExplicitFVLinearProblem::set_up_callbacks()
{
    _F_;
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitFVLinearProblem::set_up_initial_guess()
{
    _F_;
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    DiscreteProblemInterface::set_up_initial_guess();
}

void
ExplicitFVLinearProblem::set_up_time_scheme()
{
    _F_;
    ExplicitProblemInterface::set_up_time_scheme();
}

void
ExplicitFVLinearProblem::set_up_monitors()
{
    _F_;
    NonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

PetscErrorCode
ExplicitFVLinearProblem::compute_rhs_local(Real time, const Vector & x, Vector & F)
{
    _F_;
    return DMPlexTSComputeRHSFunctionFVM(get_dm(), time, x, F, this);
}

void
ExplicitFVLinearProblem::post_step()
{
    _F_;
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

} // namespace godzilla
