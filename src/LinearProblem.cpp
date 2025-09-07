// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/LinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/RestartFile.h"
#include "godzilla/RestartInterface.h"

namespace godzilla {

ErrorCode
LinearProblem::invoke_compute_operators_delegate(KSP, Mat A, Mat B, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(Matrix &, Matrix &)> *>(context);
    Matrix mat_A(A);
    mat_A.inc_reference();
    Matrix mat_B(B);
    mat_B.inc_reference();
    delegate->invoke(mat_A, mat_B);
    return 0;
}

ErrorCode
LinearProblem::invoke_compute_rhs_delegate(KSP, Vec b, void * context)
{
    CALL_STACK_MSG();
    auto * delegate = static_cast<Delegate<void(Vector &)> *>(context);
    Vector vec_b(b);
    vec_b.inc_reference();
    delegate->invoke(vec_b);
    return 0;
}

Parameters
LinearProblem::parameters()
{
    auto params = Problem::parameters();
    params.add_param<std::string>("ksp_type", "gmres", "KSP type")
        .add_param<Real>("lin_rel_tol",
                         1e-5,
                         "Relative convergence tolerance for the linear solver")
        .add_param<Real>("lin_abs_tol",
                         1e-50,
                         "Absolute convergence tolerance for the linear solver")
        .add_param<Int>("lin_max_iter",
                        10000,
                        "Maximum number of iterations for the linear solver");
    return params;
}

LinearProblem::LinearProblem(const Parameters & parameters) :
    Problem(parameters),
    RestartInterface(),
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
}

const std::string &
LinearProblem::get_ksp_type() const
{
    CALL_STACK_MSG();
    return this->ksp_type;
}

void
LinearProblem::create()
{
    CALL_STACK_MSG();
    set_up_types();
    this->ks = create_krylov_solver();
    init();
    allocate_objects();
    set_up_matrix_properties();
    this->pcond = create_preconditioner(this->ks.get_pc());
    this->pcond.inc_reference();
    set_up_solver_parameters();
    set_up_monitors();
    set_up_callbacks();
    Problem::create();
    this->ks.set_from_options();
}

KrylovSolver
LinearProblem::create_krylov_solver()
{
    CALL_STACK_MSG();
    KrylovSolver krylov_solver;
    krylov_solver.create(get_comm());
    krylov_solver.set_type(this->ksp_type);
    krylov_solver.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
    return krylov_solver;
}

const KrylovSolver &
LinearProblem::get_ksp() const
{
    CALL_STACK_MSG();
    return this->ks;
}

void
LinearProblem::init()
{
    CALL_STACK_MSG();
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
    lprintln(8, "{} Linear residual: {:e}", it, rnorm);
}

void
LinearProblem::solve()
{
    CALL_STACK_MSG();
    lprintln(9, "Solving");
    this->ks.solve(get_solution_vector());
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
    pre_solve();
    solve();
    post_solve();
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

void
LinearProblem::pre_solve()
{
    CALL_STACK_MSG();
}

void
LinearProblem::post_solve()
{
    CALL_STACK_MSG();
    if (converged())
        on_final();
}

void
LinearProblem::write_restart_file(RestartFile & file) const
{
    CALL_STACK_MSG();
    const auto & sln = get_solution_vector();
    file.write(get_name(), "/", "sln", sln);
}

void
LinearProblem::read_restart_file(const RestartFile & file)
{
    CALL_STACK_MSG();
    auto & sln = get_solution_vector();
    file.read(get_name(), "/", "sln", sln);
}

} // namespace godzilla
