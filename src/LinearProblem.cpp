// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/LinearProblem.h"
#include "godzilla/CallStack.h"

namespace godzilla {

ErrorCode
LinearProblem::invoke_compute_operators_delegate(KSP, Mat A, Mat B, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(Matrix &, Matrix &)> *>(context);
    Matrix mat_A(A);
    Matrix mat_B(B);
    delegate->invoke(mat_A, mat_B);
    return 0;
}

ErrorCode
LinearProblem::invoke_compute_rhs_delegate(KSP, Vec b, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(Vector &)> *>(context);
    Vector vec_b(b);
    delegate->invoke(vec_b);
    return 0;
}

Parameters
LinearProblem::parameters()
{
    Parameters params = Problem::parameters();
    params.add_param<std::string>("ksp_type", "gmres", "KSP type");
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
    pc(nullptr),
    ksp_type(get_param<std::string>("ksp_type")),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_FINAL);
}

LinearProblem::~LinearProblem()
{
    CALL_STACK_MSG();
    this->ks.destroy();
}

void
LinearProblem::create()
{
    CALL_STACK_MSG();
    set_up_types();
    init();
    allocate_objects();
    set_up_matrix_properties();
    this->pc = create_preconditioner(this->ks.get_pc());
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
    this->ks.set_type(this->ksp_type.c_str());
    this->ks.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
}

void
LinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    Problem::allocate_objects();
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

void
LinearProblem::monitor(Int it, Real rnorm)
{
    CALL_STACK_MSG();
    lprint(8, "{} Linear residual: {:e}", it, rnorm);
}

void
LinearProblem::solve()
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    this->ks.solve(get_solution_vector());
}

void
LinearProblem::solve(const Vector & b, Vector & x)
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    this->ks.solve(b, x);
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

Preconditioner
LinearProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();
    return Preconditioner(pc);
}

} // namespace godzilla
