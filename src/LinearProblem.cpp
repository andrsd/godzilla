// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/LinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Mesh.h"
#include "godzilla/Output.h"

namespace godzilla {

Parameters
LinearProblem::parameters()
{
    Parameters params = Problem::parameters();
    params.add_param<Real>("lin_rel_tol",
                           1e-5,
                           "Relative convergence tolerance for the linear solver");
    params.add_param<Real>("lin_abs_tol",
                           1e-50,
                           "Absolute convergence tolerance for the linear solver");
    params.add_param<Int>("lin_max_iter",
                          10000,
                          "Maximum number of iterations for the linear solver");
    return params;
}

LinearProblem::LinearProblem(const Parameters & parameters) :
    Problem(parameters),
    precond(nullptr),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
{
    CALL_STACK_MSG();
    set_default_output_on(ExecuteOn::FINAL);
}

LinearProblem::~LinearProblem()
{
    CALL_STACK_MSG();
    this->ks.destroy();
    this->b.destroy();
}

void
LinearProblem::create()
{
    CALL_STACK_MSG();
    {
        TIMED_EVENT(9, "MeshDistribution", "Distributing");
        get_mesh()->distribute();
    }
    init();
    allocate_objects();
    set_up_matrix_properties();
    set_up_preconditioning();

    set_up_solver_parameters();
    set_up_monitors();
    set_up_callbacks();

    Problem::create();

    this->ks.set_from_options();
}

void
LinearProblem::init()
{
    CALL_STACK_MSG();
    this->ks.create(get_comm());
    this->ks.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
}

void
LinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    Problem::allocate_objects();

    this->b = get_solution_vector().duplicate();
    this->b.set_name("rhs");
}

void
LinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    this->ks.set_compute_rhs(this, &LinearProblem::compute_rhs);
    this->ks.set_compute_operators(this, &LinearProblem::compute_operators);
}

void
LinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    this->ks.monitor_set(this, &LinearProblem::monitor);
}

void
LinearProblem::set_up_solver_parameters()
{
    CALL_STACK_MSG();
    this->ks.set_tolerances(this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
}

PetscErrorCode
LinearProblem::monitor(Int it, Real rnorm)
{
    CALL_STACK_MSG();
    lprint(8, "{} Linear residual: {:e}", it, rnorm);
    return 0;
}

bool
LinearProblem::converged()
{
    CALL_STACK_MSG();
    return this->ks.get_converged_reason() > 0;
}

void
LinearProblem::run()
{
    CALL_STACK_MSG();
    solve();
    if (converged())
        on_final();
}

void
LinearProblem::set_up_matrix_properties()
{
    CALL_STACK_MSG();
}

void
LinearProblem::set_up_preconditioning()
{
    CALL_STACK_MSG();
    auto pc = this->ks.get_pc();
    this->precond = create_preconditioner(pc);
}

Preconditioner
LinearProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();
    return Preconditioner(pc);
}

void
LinearProblem::solve()
{
    CALL_STACK_MSG();
    this->ks.solve(this->b, get_solution_vector());
}

PetscErrorCode
LinearProblem::compute_rhs(Vector &)
{
    CALL_STACK_MSG();
    return 0;
}

PetscErrorCode
LinearProblem::compute_operators(Matrix &, Matrix &)
{
    CALL_STACK_MSG();
    return 0;
}

} // namespace godzilla
