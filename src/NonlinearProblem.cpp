// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NonlinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/Convert.h"
#include "godzilla/RestartFile.h"
#include "godzilla/Validation.h"

namespace godzilla {

Parameters
NonlinearProblem::parameters()
{
    auto params = Problem::parameters();
    params.add_param<String>("line_search", "bt", "The type of line search to be used")
        .add_param<Real>("nl_rel_tol",
                         1e-8,
                         "Relative convergence tolerance for the non-linear solver")
        .add_param<Real>("nl_abs_tol",
                         1e-15,
                         "Absolute convergence tolerance for the non-linear solver")
        .add_param<Real>("nl_step_tol",
                         1e-15,
                         "Convergence tolerance in terms of the norm of the change in the solution "
                         "between steps")
        .add_param<Int>("nl_max_iter", 40, "Maximum number of iterations for the non-linear solver")
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

NonlinearProblem::NonlinearProblem(const Parameters & pars) :
    Problem(pars),
    snes_(),
    ksp_(),
    line_search_type_(pars.get<String>("line_search")),
    nl_rel_tol_(pars.get<Real>("nl_rel_tol")),
    nl_abs_tol_(pars.get<Real>("nl_abs_tol")),
    nl_step_tol_(pars.get<Real>("nl_step_tol")),
    nl_max_iter_(pars.get<Int>("nl_max_iter")),
    lin_rel_tol_(pars.get<Real>("lin_rel_tol")),
    lin_abs_tol_(pars.get<Real>("lin_abs_tol")),
    lin_max_iter_(pars.get<Int>("lin_max_iter"))
{
    CALL_STACK_MSG();
    this->line_search_type_ = this->line_search_type_.to_lower();
#if PETSC_VERSION_GE(3, 24, 0)
    expect_true(validation::in(this->line_search_type_,
                               { "bt", "basic", "secant", "cp", "nleqerr", "shell" }),
                "The 'line_search' parameter can be either 'bt', 'basic', 'secant', 'cp', "
                "'nleqerr' or 'shell'.");
#else
    expect_true(
        validation::in(this->line_search_type, { "bt", "basic", "l2", "cp", "nleqerr", "shell" }),
        "The 'line_search' parameter can be either 'bt', 'basic', 'l2', 'cp', 'nleqerr' or "
        "'shell'.");
#endif
}

const Matrix &
NonlinearProblem::get_jacobian() const
{
    CALL_STACK_MSG();
    return this->J_;
}

Matrix &
NonlinearProblem::get_jacobian()
{
    CALL_STACK_MSG();
    return this->J_;
}

void
NonlinearProblem::create()
{
    CALL_STACK_MSG();
    set_up_types();
    init();
    allocate_objects();
    set_up_matrix_properties();
    this->pcond_ = create_preconditioner(this->ksp_.get_pc());
    this->pcond_.inc_reference();
    set_up_solver_parameters();
    set_up_line_search();
    set_up_monitors();
    set_up_callbacks();
    set_up_solve_type();
    Problem::create();
}

const SNESolver &
NonlinearProblem::get_snes() const
{
    CALL_STACK_MSG();
    return this->snes_;
}

SNESolver &
NonlinearProblem::get_snes()
{
    CALL_STACK_MSG();
    return this->snes_;
}

void
NonlinearProblem::set_residual_vector(const Vector & f)
{
    CALL_STACK_MSG();
    this->r_ = f;
    this->r_.set_name("res");
}

void
NonlinearProblem::set_jacobian_matrix(const Matrix & J)
{
    CALL_STACK_MSG();
    this->J_ = J;
    this->J_.set_name("Jac");
}

const KrylovSolver &
NonlinearProblem::get_ksp() const
{
    CALL_STACK_MSG();
    return this->ksp_;
}

KrylovSolver &
NonlinearProblem::get_ksp()
{
    CALL_STACK_MSG();
    return this->ksp_;
}

SNESolver
NonlinearProblem::create_sne_solver()
{
    CALL_STACK_MSG();
    SNESolver snes;
    snes.create(get_comm());
    snes.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
    return snes;
}

void
NonlinearProblem::init()
{
    CALL_STACK_MSG();
    this->snes_ = create_sne_solver();
    PETSC_CHECK(SNESGetKSP(this->snes_, this->ksp_));
    this->ksp_.inc_reference();
}

void
NonlinearProblem::set_use_matrix_free(bool mf_operator, bool mf)
{
    CALL_STACK_MSG();
    this->snes_.set_use_matrix_free(mf_operator, mf);
}

void
NonlinearProblem::set_initial_guess()
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
    auto ls = this->snes_.get_line_search();
    if (this->line_search_type_ == "basic")
        ls.set_type(SNESolver::LineSearchType::BASIC);
    else if (this->line_search_type_ == "l2")
        ls.set_type(SNESolver::LineSearchType::L2);
    else if (this->line_search_type_ == "cp")
        ls.set_type(SNESolver::LineSearchType::CP);
    else if (this->line_search_type_ == "nleqerr")
        ls.set_type(SNESolver::LineSearchType::NLEQERR);
    else if (this->line_search_type_ == "shell")
        ls.set_type(SNESolver::LineSearchType::SHELL);
#if PETSC_VERSION_GE(3, 24, 0)
    else if (this->line_search_type_ == "secant")
        ls.set_type(SNESolver::LineSearchType::SECANT);
#endif
    else
        ls.set_type(SNESolver::LineSearchType::BT);
    this->snes_.set_line_search(ls);
    ls.set_from_options();
}

void
NonlinearProblem::set_up_callbacks()
{
    CALL_STACK_MSG();
}

void
NonlinearProblem::set_up_monitors()
{
    CALL_STACK_MSG();
    this->snes_.monitor_set(ref(*this), &NonlinearProblem::snes_monitor);
    this->snes_.converged_reason_view_set(ref(*this),
                                          &NonlinearProblem::snes_converged_reason_view);
    this->ksp_.monitor_set(ref(*this), &NonlinearProblem::ksp_monitor);
    this->ksp_.converged_reason_view_set(ref(*this), &NonlinearProblem::ksp_converged_reason_view);
}

void
NonlinearProblem::set_up_solve_type()
{
}

void
NonlinearProblem::set_up_solver_parameters()
{
    CALL_STACK_MSG();
    this->snes_.set_tolerances(this->nl_abs_tol_,
                               this->nl_rel_tol_,
                               this->nl_step_tol_,
                               this->nl_max_iter_,
                               -1);
    this->snes_.set_from_options();

    this->ksp_.set_tolerances(this->lin_rel_tol_,
                              this->lin_abs_tol_,
                              PETSC_DEFAULT,
                              this->lin_max_iter_);
    this->ksp_.set_from_options();
}

void
NonlinearProblem::snes_monitor(Int it, Real norm)
{
    CALL_STACK_MSG();
    lprintln(6, "{} Non-linear residual: {:e}", it, norm);
}

void
NonlinearProblem::ksp_monitor(Int it, Real rnorm)
{
    CALL_STACK_MSG();
    lprintln(8, "    {} Linear residual: {:e}", it, rnorm);
}

void
NonlinearProblem::snes_converged_reason_view()
{
    CALL_STACK_MSG();
    auto & snes = get_snes();
    auto reason = snes.get_converged_reason();
    auto n_iters = snes.get_iteration_number();
    if (reason > 0)
        lprintln(5,
                 Terminal::green,
                 "Non-linear solver converged: {} ({} iterations)",
                 conv::to_str(reason),
                 utils::human_number(n_iters));
    else
        lprintln(5, Terminal::red, "Non-linear solver diverged: {}", conv::to_str(reason));
}

void
NonlinearProblem::ksp_converged_reason_view()
{
    CALL_STACK_MSG();
    auto & ksp = get_ksp();
    auto reason = ksp.get_converged_reason();
    auto n_iters = ksp.get_iteration_number();
    if (reason > 0)
        lprintln(7,
                 Terminal::green,
                 "    Linear solver converged: {} ({} iterations)",
                 conv::to_str(reason),
                 utils::human_number(n_iters));
    else
        lprintln(7, Terminal::red, "    Linear solver diverged: {}", conv::to_str(reason));
}

void
NonlinearProblem::set_ksp_operators(const Matrix & A, const Matrix & B)
{
    CALL_STACK_MSG();
    this->ksp_.set_operators(A, B);
}

void
NonlinearProblem::solve()
{
    CALL_STACK_MSG();
    lprintln(9, "Solving");
    this->snes_.solve(get_solution_vector());
}

bool
NonlinearProblem::converged()
{
    CALL_STACK_MSG();
    auto reason = this->snes_.get_converged_reason();
    bool conv = (reason == SNESolver::CONVERGED_FNORM_ABS) ||
                (reason == SNESolver::CONVERGED_FNORM_RELATIVE) ||
                (reason == SNESolver::CONVERGED_SNORM_RELATIVE) ||
                (reason == SNESolver::CONVERGED_ITS);
    return conv;
}

void
NonlinearProblem::run()
{
    CALL_STACK_MSG();
    set_initial_guess();
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

Preconditioner
NonlinearProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();
    return Preconditioner(pc);
}

void
NonlinearProblem::write_restart_file(RestartFile & file) const
{
    CALL_STACK_MSG();
    const auto & sln = get_solution_vector();
    file.write_global_vector(get_name(), "/", "sln", sln);
}

void
NonlinearProblem::read_restart_file(const RestartFile & file)
{
    CALL_STACK_MSG();
    auto & sln = get_solution_vector();
    file.read_global_vector(get_name(), "/", "sln", sln);
}

} // namespace godzilla
