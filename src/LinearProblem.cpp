// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/LinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Mesh.h"
#include "godzilla/Output.h"

namespace godzilla {

namespace internal {

PetscErrorCode
compute_rhs(KSP, Vec b, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    Vector vec_b(b);
    return problem->compute_rhs(vec_b);
}

PetscErrorCode
compute_operators(KSP, Mat A, Mat B, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    Matrix mat_A(A);
    Matrix mat_B(B);
    return problem->compute_operators(mat_A, mat_B);
}

PetscErrorCode
ksp_monitor(KSP, Int it, Real rnorm, void * ctx)
{
    _F_;
    auto * problem = static_cast<LinearProblem *>(ctx);
    return problem->monitor(it, rnorm);
}

} // namespace internal

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
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
{
    _F_;
    set_default_output_on(ExecuteOn::FINAL);
}

LinearProblem::~LinearProblem()
{
    _F_;
    this->ks.destroy();
    this->b.destroy();
}

void
LinearProblem::create()
{
    _F_;
    get_mesh()->distribute();
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
    _F_;
    this->ks.create(get_comm());
    this->ks.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
}

void
LinearProblem::allocate_objects()
{
    _F_;
    Problem::allocate_objects();

    this->b = get_solution_vector().duplicate();
    this->b.set_name("rhs");
}

void
LinearProblem::set_up_callbacks()
{
    _F_;
    this->ks.set_compute_rhs(internal::compute_rhs, this);
    this->ks.set_compute_operators(internal::compute_operators, this);
    // TODO: this would be nice
    // this->ks.set_compute_rhs(this, &LinearProblem::compute_rhs);
    // this->ks.set_compute_operators(this, &LinearProblem::compute_operators);
}

void
LinearProblem::set_up_monitors()
{
    _F_;
    this->ks.monitor_set(internal::ksp_monitor, this);
    // TODO: this would be nice
    // this->ks.monitor_set(this, &LinearProblem::monitor);
}

void
LinearProblem::set_up_solver_parameters()
{
    _F_;
    this->ks.set_tolerances(this->lin_rel_tol,
                            this->lin_abs_tol,
                            PETSC_DEFAULT,
                            this->lin_max_iter);
}

PetscErrorCode
LinearProblem::monitor(Int it, Real rnorm)
{
    _F_;
    lprint(8, "{} Linear residual: {:e}", it, rnorm);
    return 0;
}

bool
LinearProblem::converged()
{
    _F_;
    return this->ks.get_converged_reason() > 0;
}

void
LinearProblem::run()
{
    _F_;
    solve();
    if (converged())
        on_final();
}

void
LinearProblem::set_up_matrix_properties()
{
    _F_;
}

void
LinearProblem::set_up_preconditioning()
{
    _F_;
}

void
LinearProblem::solve()
{
    _F_;
    this->ks.solve(this->b, get_solution_vector());
}

PetscErrorCode
LinearProblem::compute_rhs(Vector &)
{
    _F_;
    return 0;
}

PetscErrorCode
LinearProblem::compute_operators(Matrix &, Matrix &)
{
    _F_;
    return 0;
}

} // namespace godzilla
