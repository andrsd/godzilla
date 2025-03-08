// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/NonlinearProblem.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/RestartFile.h"
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
    line_search_type(get_param<std::string>("line_search")),
    nl_rel_tol(get_param<Real>("nl_rel_tol")),
    nl_abs_tol(get_param<Real>("nl_abs_tol")),
    nl_step_tol(get_param<Real>("nl_step_tol")),
    nl_max_iter(get_param<Int>("nl_max_iter")),
    lin_rel_tol(get_param<Real>("lin_rel_tol")),
    lin_abs_tol(get_param<Real>("lin_abs_tol")),
    lin_max_iter(get_param<Int>("lin_max_iter")),
    my_snes(false)
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
    if (this->my_snes)
        this->snes.destroy();
    this->r.destroy();
    this->J.destroy();
}

const Matrix &
NonlinearProblem::get_jacobian() const
{
    CALL_STACK_MSG();
    return this->J;
}

Matrix &
NonlinearProblem::get_jacobian()
{
    CALL_STACK_MSG();
    return this->J;
}

void
NonlinearProblem::create()
{
    CALL_STACK_MSG();
    set_up_types();
    init();
    allocate_objects();
    set_up_matrix_properties();
    create_preconditioner(this->ksp.get_pc());
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

SNESolver
NonlinearProblem::create_sne_solver()
{
    CALL_STACK_MSG();
    SNESolver snes;
    snes.create(get_comm());
    snes.set_dm(get_dm());
    PETSC_CHECK(DMSetApplicationContext(get_dm(), this));
    this->my_snes = true;
    return snes;
}

void
NonlinearProblem::init()
{
    CALL_STACK_MSG();
    set_snes(create_sne_solver());
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
    auto ls = this->snes.get_line_search();
    if (this->line_search_type == "basic")
        ls.set_type(SNESolver::LineSearch::BASIC);
    else if (this->line_search_type == "l2")
        ls.set_type(SNESolver::LineSearch::L2);
    else if (this->line_search_type == "cp")
        ls.set_type(SNESolver::LineSearch::CP);
    else if (this->line_search_type == "nleqerr")
        ls.set_type(SNESolver::LineSearch::NLEQERR);
    else if (this->line_search_type == "shell")
        ls.set_type(SNESolver::LineSearch::SHELL);
    else
        ls.set_type(SNESolver::LineSearch::BT);
    this->snes.set_line_search(ls);
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

void
NonlinearProblem::snes_monitor(Int it, Real norm)
{
    CALL_STACK_MSG();
    lprintln(7, "{} Non-linear residual: {:e}", it, norm);
}

void
NonlinearProblem::ksp_monitor(Int it, Real rnorm)
{
    CALL_STACK_MSG();
    lprintln(8, "    {} Linear residual: {:e}", it, rnorm);
}

void
NonlinearProblem::set_ksp_operators(const Matrix & A, const Matrix & B)
{
    CALL_STACK_MSG();
    this->ksp.set_operators(A, B);
}

void
NonlinearProblem::solve()
{
    CALL_STACK_MSG();
    lprintln(9, "Solving");
    this->snes.solve(get_solution_vector());
}

bool
NonlinearProblem::converged()
{
    CALL_STACK_MSG();
    auto reason = this->snes.get_converged_reason();
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
    pre_solve();
    set_up_initial_guess();
    on_initial();
    solve();
    if (converged())
        on_final();
    post_solve();
}

void
NonlinearProblem::set_up_matrix_properties()
{
    CALL_STACK_MSG();
}

void
NonlinearProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();
    this->pcond = Preconditioner(pc);
}

void
NonlinearProblem::pre_solve()
{
    CALL_STACK_MSG();
}

void
NonlinearProblem::post_solve()
{
    CALL_STACK_MSG();
}

void
NonlinearProblem::write_restart_file(RestartFile & file) const
{
    CALL_STACK_MSG();
    const auto & sln = get_solution_vector();
    file.write(get_name(), "sln", sln);
}

void
NonlinearProblem::read_restart_file(const RestartFile & file)
{
    CALL_STACK_MSG();
    auto & sln = get_solution_vector();
    file.read(get_name(), "sln", sln);
}

} // namespace godzilla
