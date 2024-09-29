// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/SNESolver.h"
#include "godzilla/CallStack.h"

namespace godzilla {

SNESolver::LineSearch::LineSearch(SNESLineSearch ls) : ls(ls) {}

void
SNESolver::LineSearch::set_type(LineSearchType type)
{
    CALL_STACK_MSG();
    switch (type) {
    case BASIC:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHBASIC));
        return;

    case L2:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHL2));
        return;

    case CP:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHCP));
        return;

    case NLEQERR:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHNLEQERR));
        return;

    case SHELL:
        PETSC_CHECK(SNESLineSearchSetType(this->ls, SNESLINESEARCHSHELL));
        return;

    case BT:
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

ErrorCode
SNESolver::invoke_compute_residual_delegate(SNES, Vec x, Vec f, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(const Vector & x, Vector & f)> *>(ctx);
    Vector vec_x(x);
    Vector vec_f(f);
    method->invoke(vec_x, vec_f);
    return 0;
}

ErrorCode
SNESolver::invoke_compute_jacobian_delegate(SNES, Vec x, Mat J, Mat Jp, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(const Vector & x, Matrix & J, Matrix & Jp)> *>(ctx);
    Vector vec_x(x);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    method->invoke(vec_x, mat_J, mat_Jp);
    return 0;
}

ErrorCode
SNESolver::invoke_monitor_delegate(SNES, Int it, Real rnorm, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Int it, Real rnorm)> *>(ctx);
    method->invoke(it, rnorm);
    return 0;
}

SNESolver::SNESolver() : snes(nullptr) {}

SNESolver::SNESolver(SNES snes) : snes(snes) {}

void
SNESolver::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESCreate(comm, &this->snes));
}

void
SNESolver::destroy()
{
    CALL_STACK_MSG();
    if (this->snes) {
        SNESDestroy(&this->snes);
        this->snes = nullptr;
    }
}

KrylovSolver
SNESolver::get_ksp() const
{
    CALL_STACK_MSG();
    KSP ksp;
    PETSC_CHECK(SNESGetKSP(this->snes, &ksp));
    return KrylovSolver(ksp);
}

SNESolver::LineSearch
SNESolver::get_line_search() const
{
    CALL_STACK_MSG();
    SNESLineSearch ls;
    PETSC_CHECK(SNESGetLineSearch(this->snes, &ls));
    return LineSearch(ls);
}

void
SNESolver::set_line_search(SNESolver::LineSearch ls)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetLineSearch(this->snes, ls));
}

void
SNESolver::set_dm(DM dm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetDM(this->snes, dm));
}

void
SNESolver::set_type(SNESType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetType(this->snes, type));
}

std::string
SNESolver::get_type() const
{
    CALL_STACK_MSG();
    SNESType type;
    PETSC_CHECK(SNESGetType(this->snes, &type));
    return std::string(type);
}

void
SNESolver::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetFromOptions(this->snes));
}

void
SNESolver::set_use_matrix_free(bool mf_operator, bool mf)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetUseMatrixFree(this->snes,
                                     mf_operator ? PETSC_TRUE : PETSC_FALSE,
                                     mf ? PETSC_TRUE : PETSC_FALSE));
}

void
SNESolver::set_tolerances(Real abs_tol, Real rtol, Real stol, Int max_it, Int maxf)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetTolerances(this->snes, abs_tol, rtol, stol, max_it, maxf));
}

void
SNESolver::solve(const Vector & b, Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSolve(this->snes, b, x));
}

void
SNESolver::solve(Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSolve(this->snes, nullptr, x));
}

SNESolver::ConvergedReason
SNESolver::get_converged_reason() const
{
    CALL_STACK_MSG();
    SNESConvergedReason reason;
    PETSC_CHECK(SNESGetConvergedReason(this->snes, &reason));
    return static_cast<ConvergedReason>(reason);
}

Matrix
SNESolver::mat_create_mf() const
{
    CALL_STACK_MSG();
    Mat mat;
    PETSC_CHECK(MatCreateSNESMF(this->snes, &mat));
    return { mat };
}

SNESolver::operator SNES() const
{
    CALL_STACK_MSG();
    return this->snes;
}

} // namespace godzilla
