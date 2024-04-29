// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NonlinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/Mesh.h"
#include "godzilla/Output.h"
#include "godzilla/Validation.h"
#include "petscds.h"

namespace godzilla {

Parameters
NonlinearProblem::parameters()
{
    Parameters params = Problem::parameters();
    params.add_param<std::string>("line_search", "bt", "The type of line search to be used");
    params.add_param<Real>("nl_rel_tol",
                           1e-8,
                           "Relative convergence tolerance for the non-linear solver");
    params.add_param<Real>("nl_abs_tol",
                           1e-15,
                           "Absolute convergence tolerance for the non-linear solver");
    params.add_param<Real>(
        "nl_step_tol",
        1e-15,
        "Convergence tolerance in terms of the norm of the change in the solution between steps");
    params.add_param<Int>("nl_max_iter",
                          40,
                          "Maximum number of iterations for the non-linear solver");
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

NonlinearProblem::NonlinearProblem(const Parameters & parameters) :
    Problem(parameters),
    snes(),
    ksp(),
    converged_reason(SNESolver::CONVERGED_ITERATING),
    line_search_type(get_param<std::string>("line_search")),
    nl_rel_tol(get_param<Real>("nl_rel_tol")),
    nl_abs_tol(get_param<Real>("nl_abs_tol")),
    nl_step_tol(get_param<Real>("nl_step_tol")),
    nl_max_iter(get_param<Int>("nl_max_iter")),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter"))
{
    CALL_STACK_MSG();
    set_default_output_on(EXECUTE_ON_FINAL);
    this->line_search_type = utils::to_lower(line_search_type);
    if (!validation::in(this->line_search_type, { "bt", "basic", "l2", "cp", "nleqerr", "shell" }))
        log_error("The 'line_search' parameter can be either 'bt', 'basic', 'l2', 'cp', 'nleqerr' "
                  "or 'shell'.");
}

NonlinearProblem::~NonlinearProblem()
{
    CALL_STACK_MSG();
    this->r.destroy();
    this->J.destroy();
}

const Matrix &
NonlinearProblem::get_jacobian() const
{
    CALL_STACK_MSG();
    return this->J;
}

void
NonlinearProblem::create()
{
    CALL_STACK_MSG();
    init();
    allocate_objects();
    set_up_matrix_properties();
    set_up_preconditioning();

    set_up_solver_parameters();
    set_up_line_search();
    set_up_monitors();
    set_up_callbacks();
    set_up_solve_type();

    Problem::create();
}

SNESolver
NonlinearProblem::get_snes() const
{
    CALL_STACK_MSG();
    return this->snes;
}

void
NonlinearProblem::set_snes(const SNESolver & snes)
{
    CALL_STACK_MSG();
    this->snes = snes;
    this->ksp = snes.get_ksp();
}

void
NonlinearProblem::set_residual_vector(const Vector & f)
{
    CALL_STACK_MSG();
    this->r = f;
    this->r.set_name("res");
}

void
NonlinearProblem::set_jacobian_matrix(const Matrix & J)
{
    CALL_STACK_MSG();
    this->J = J;
    this->J.set_name("Jac");
}

KrylovSolver
NonlinearProblem::get_ksp() const
{
    CALL_STACK_MSG();
    return this->ksp;
}

void
NonlinearProblem::init()
{
    CALL_STACK_MSG();
    DM dm = get_dm();
    this->snes.create(get_comm());
    this->snes.set_dm(dm);
    PETSC_CHECK(DMSetApplicationContext(dm, this));
    set_snes(this->snes);
}

void
NonlinearProblem::set_use_matrix_free(bool mf_operator, bool mf)
{
    CALL_STACK_MSG();
    this->snes.set_use_matrix_free(mf_operator, mf);
}

void
NonlinearProblem::set_up_initial_guess()
{
    CALL_STACK_MSG();
    TIMED_EVENT(9, "InitialGuess", "Setting initial guess");
    get_solution_vector().set(0.);
}

void
NonlinearProblem::allocate_objects()
{
    CALL_STACK_MSG();
    Problem::allocate_objects();
    set_residual_vector(get_solution_vector().duplicate());
    set_jacobian_matrix(create_matrix());
}

void
NonlinearProblem::set_up_line_search()
{
    CALL_STACK_MSG();
    SNESLineSearch line_search;
    PETSC_CHECK(SNESGetLineSearch(this->snes, &line_search));
    if (this->line_search_type == "basic")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHBASIC));
    else if (this->line_search_type == "l2")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHL2));
    else if (this->line_search_type == "cp")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHCP));
    else if (this->line_search_type == "nleqerr")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHNLEQERR));
    else if (this->line_search_type == "shell")
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHSHELL));
    else
        PETSC_CHECK(SNESLineSearchSetType(line_search, SNESLINESEARCHBT));
    PETSC_CHECK(SNESSetLineSearch(this->snes, line_search));
    PETSC_CHECK(SNESLineSearchSetFromOptions(line_search));
}

void
NonlinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
    this->snes.set_function(this->r, this, &NonlinearProblem::compute_residual);
    this->snes.set_jacobian(this->J, this->J, this, &NonlinearProblem::compute_jacobian);
}

void
NonlinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    this->snes.monitor_set(this, &NonlinearProblem::snes_monitor);
    this->ksp.monitor_set(this, &NonlinearProblem::ksp_monitor);
}

void
NonlinearProblem::set_up_solve_type()
{
}

void
NonlinearProblem::set_up_solver_parameters()
{
    CALL_STACK_MSG();
    this->snes.set_tolerances(this->nl_abs_tol,
                              this->nl_rel_tol,
                              this->nl_step_tol,
                              this->nl_max_iter,
                              -1);
    this->snes.set_from_options();

    this->ksp.set_tolerances(this->lin_rel_tol,
                             this->lin_abs_tol,
                             PETSC_DEFAULT,
                             this->lin_max_iter);
    this->ksp.set_from_options();
}

PetscErrorCode
NonlinearProblem::snes_monitor(Int it, Real norm)
{
    CALL_STACK_MSG();
    lprint(7, "{} Non-linear residual: {:e}", it, norm);
    return 0;
}

PetscErrorCode
NonlinearProblem::ksp_monitor(Int it, Real rnorm)
{
    CALL_STACK_MSG();
    lprint(8, "    {} Linear residual: {:e}", it, rnorm);
    return 0;
}

void
NonlinearProblem::set_ksp_operators(const Matrix & A, const Matrix & B)
{
    CALL_STACK_MSG();
    this->ksp.set_operators(A, B);
}

bool
NonlinearProblem::converged()
{
    CALL_STACK_MSG();
    bool conv = (this->converged_reason == SNESolver::CONVERGED_FNORM_ABS) ||
                (this->converged_reason == SNESolver::CONVERGED_FNORM_RELATIVE) ||
                (this->converged_reason == SNESolver::CONVERGED_SNORM_RELATIVE) ||
                (this->converged_reason == SNESolver::CONVERGED_ITS);
    return conv;
}

void
NonlinearProblem::run()
{
    CALL_STACK_MSG();
    set_up_initial_guess();
    on_initial();
    solve();
    if (converged())
        on_final();
}

void
NonlinearProblem::set_up_matrix_properties()
{
    CALL_STACK_MSG();
}

void
NonlinearProblem::set_up_preconditioning()
{
    CALL_STACK_MSG();
    auto pc = this->ksp.get_pc();
    this->precond = create_preconditioner(pc);
}

Preconditioner
NonlinearProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();
    return Preconditioner(pc);
}

void
NonlinearProblem::solve()
{
    CALL_STACK_MSG();
    lprint(9, "Solving");
    this->snes.solve(get_solution_vector());
    this->converged_reason = this->snes.get_converged_reason();
}

PetscErrorCode
NonlinearProblem::compute_residual(const Vector &, Vector &)
{
    CALL_STACK_MSG();
    return 0;
}

PetscErrorCode
NonlinearProblem::compute_jacobian(const Vector &, Matrix &, Matrix &)
{
    CALL_STACK_MSG();
    return 0;
}

} // namespace godzilla
