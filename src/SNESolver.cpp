// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/SNESolver.h"
#include "godzilla/CallStack.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Convert.h"

namespace godzilla {

SNESolver::LineSearch::LineSearch(SNESLineSearch ls) : ls(ls) {}

void
SNESolver::LineSearch::set_type(LineSearchType type)
{
    CALL_STACK_MSG();
    switch (type) {
    case LineSearchType::BASIC:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHBASIC));
        return;

#if PETSC_VERSION_GE(3, 24, 0)
    case LineSearchType::SECANT:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHSECANT));
        return;
#endif

    case LineSearchType::L2:
#if PETSC_VERSION_GE(3, 24, 0)
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHSECANT));
#else
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHL2));
#endif
        return;

    case LineSearchType::CP:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHCP));
        return;

    case LineSearchType::NLEQERR:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHNLEQERR));
        return;

    case LineSearchType::SHELL:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHSHELL));
        return;

    case LineSearchType::BT:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHBT));
        return;
    }
}

void
SNESolver::LineSearch::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESLineSearchSetFromOptions(this->ls));
}

SNESolver::LineSearch::operator SNESLineSearch() const
{
    CALL_STACK_MSG();
    return this->ls;
}

//

PetscErrorCode
SNESolver::invoke_compute_residual_delegate(SNES, Vec x, Vec f, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(const Vector & x, Vector & f)> *>(ctx);
    Vector vec_x(x);
    vec_x.inc_reference();
    Vector vec_f(f);
    vec_f.inc_reference();
    method->invoke(vec_x, vec_f);
    return 0;
}

PetscErrorCode
SNESolver::invoke_compute_jacobian_delegate(SNES, Vec x, Mat J, Mat Jp, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(const Vector & x, Matrix & J, Matrix & Jp)> *>(ctx);
    Vector vec_x(x);
    vec_x.inc_reference();
    Matrix mat_J(J);
    mat_J.inc_reference();
    Matrix mat_Jp(Jp);
    mat_Jp.inc_reference();
    method->invoke(vec_x, mat_J, mat_Jp);
    return 0;
}

PetscErrorCode
SNESolver::invoke_monitor_delegate(SNES, Int it, Real rnorm, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Int it, Real rnorm)> *>(ctx);
    method->invoke(it, rnorm);
    return 0;
}

PetscErrorCode
SNESolver::invoke_converged_view_delegate(SNES, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(void)> *>(ctx);
    method->invoke();
    return 0;
}

SNESolver::SNESolver() : PetscObjectWrapper(nullptr) {}

SNESolver::SNESolver(SNES snes) : PetscObjectWrapper(snes) {}

SNESolver::~SNESolver()
{
    CALL_STACK_MSG();
}

void
SNESolver::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESCreate(comm, &this->obj));
}

void
SNESolver::destroy()
{
    CALL_STACK_MSG();
}

KrylovSolver
SNESolver::get_ksp() const
{
    CALL_STACK_MSG();
    KrylovSolver ks;
    PETSC_CHECK(SNESGetKSP(this->obj, ks));
    ks.inc_reference();
    return ks;
}

SNESolver::LineSearch
SNESolver::get_line_search() const
{
    CALL_STACK_MSG();
    SNESLineSearch ls;
    PETSC_CHECK(SNESGetLineSearch(this->obj, &ls));
    return LineSearch(ls);
}

void
SNESolver::set_line_search(SNESolver::LineSearch ls)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetLineSearch(this->obj, ls));
}

void
SNESolver::set_dm(DM dm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetDM(this->obj, dm));
}

void
SNESolver::set_type(SNESType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetType(this->obj, type));
}

String
SNESolver::get_type() const
{
    CALL_STACK_MSG();
    SNESType type;
    PETSC_CHECK(SNESGetType(this->obj, &type));
    return String(type);
}

void
SNESolver::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetFromOptions(this->obj));
}

void
SNESolver::set_use_matrix_free(bool mf_operator, bool mf)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetUseMatrixFree(this->obj,
                                     mf_operator ? PETSC_TRUE : PETSC_FALSE,
                                     mf ? PETSC_TRUE : PETSC_FALSE));
}

void
SNESolver::set_tolerances(Real abs_tol, Real rtol, Real stol, Int max_it, Int maxf)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetTolerances(this->obj, abs_tol, rtol, stol, max_it, maxf));
}

void
SNESolver::solve(const Vector & b, Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSolve(this->obj, b, x));
}

void
SNESolver::solve(Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSolve(this->obj, nullptr, x));
}

SNESolver::ConvergedReason
SNESolver::get_converged_reason() const
{
    CALL_STACK_MSG();
    SNESConvergedReason reason;
    PETSC_CHECK(SNESGetConvergedReason(this->obj, &reason));
    return static_cast<ConvergedReason>(reason);
}

Int
SNESolver::get_iteration_number() const
{
    CALL_STACK_MSG();
    Int iter;
    PETSC_CHECK(SNESGetIterationNumber(this->obj, &iter));
    return iter;
}

Int
SNESolver::get_linear_solve_iterations() const
{
    CALL_STACK_MSG();
    Int iter;
    PETSC_CHECK(SNESGetLinearSolveIterations(this->obj, &iter));
    return iter;
}

Matrix
SNESolver::mat_create_mf() const
{
    CALL_STACK_MSG();
    Matrix mat;
    PETSC_CHECK(MatCreateSNESMF(this->obj, mat));
    return mat;
}

void
print_converged_reason(PrintInterface & pi, SNESolver::ConvergedReason reason)
{
    CALL_STACK_MSG();
    if (reason > 0)
        pi.lprintln(5, Terminal::green, "Converged: {}", conv::to_str(reason));
    else
        pi.lprintln(5, Terminal::red, "Not converged: {}", conv::to_str(reason));
}

} // namespace godzilla
