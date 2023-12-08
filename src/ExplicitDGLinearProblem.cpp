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
    _F_;
    set_default_output_on(ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
}

ExplicitDGLinearProblem::~ExplicitDGLinearProblem()
{
    set_snes(nullptr);
}

Real
ExplicitDGLinearProblem::get_time() const
{
    _F_;
    return ExplicitProblemInterface::get_time();
}

Int
ExplicitDGLinearProblem::get_step_num() const
{
    _F_;
    return ExplicitProblemInterface::get_step_number();
}

void
ExplicitDGLinearProblem::init()
{
    _F_;
    ExplicitProblemInterface::init();
    auto snes = ExplicitProblemInterface::get_snes();
    NonlinearProblem::set_snes(snes);
    DGProblemInterface::init();
}

void
ExplicitDGLinearProblem::create()
{
    _F_;
    DGProblemInterface::create();
    NonlinearProblem::create();
    ExplicitProblemInterface::create();
}

void
ExplicitDGLinearProblem::check()
{
    _F_;
    NonlinearProblem::check();
    ExplicitProblemInterface::check();
}

bool
ExplicitDGLinearProblem::converged()
{
    _F_;
    return ExplicitProblemInterface::converged();
}

void
ExplicitDGLinearProblem::solve()
{
    _F_;
    lprint(9, "Solving");
    ExplicitProblemInterface::solve(get_solution_vector());
}

void
ExplicitDGLinearProblem::compute_solution_vector_local()
{
    _F_;
    auto loc_sln = get_solution_vector_local();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), get_solution_vector(), INSERT_VALUES, loc_sln));
    compute_boundary_local(get_time(), loc_sln);
}

void
ExplicitDGLinearProblem::allocate_objects()
{
    _F_;
    Problem::allocate_objects();
    DGProblemInterface::allocate_objects();
}

void
ExplicitDGLinearProblem::set_up_callbacks()
{
    _F_;
    ExplicitProblemInterface::set_up_callbacks();
}

void
ExplicitDGLinearProblem::set_up_initial_guess()
{
    _F_;
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    DiscreteProblemInterface::set_up_initial_guess();
}

void
ExplicitDGLinearProblem::set_up_time_scheme()
{
    _F_;
    ExplicitProblemInterface::set_up_time_scheme();
}

void
ExplicitDGLinearProblem::set_up_monitors()
{
    _F_;
    NonlinearProblem::set_up_monitors();
    ExplicitProblemInterface::set_up_monitors();
}

void
ExplicitDGLinearProblem::post_step()
{
    _F_;
    TransientProblemInterface::post_step();
    update_aux_vector();
    compute_postprocessors();
    output(ExecuteOn::TIMESTEP);
}

} // namespace godzilla
