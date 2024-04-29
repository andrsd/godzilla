// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/SNESolver.h"

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

PetscErrorCode
SNESolver::compute_residual(SNES, Vec x, Vec f, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::SNESComputeResidualMethodAbstract *>(ctx);
    Vector vec_x(x);
    Vector vec_f(f);
    return method->invoke(vec_x, vec_f);
}

PetscErrorCode
SNESolver::compute_jacobian(SNES, Vec x, Mat J, Mat Jp, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::SNESComputeJacobianMethodAbstract *>(ctx);
    Vector vec_x(x);
    Matrix mat_J(J);
    Matrix mat_Jp(Jp);
    return method->invoke(vec_x, mat_J, mat_Jp);
}

PetscErrorCode
SNESolver::monitor(SNES, Int it, Real rnorm, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<internal::SNESMonitorMethodAbstract *>(ctx);
    return method->invoke(it, rnorm);
}

PetscErrorCode
SNESolver::monitor_destroy(void ** ctx)
{
    auto * method = static_cast<internal::SNESMonitorMethodAbstract *>(*ctx);
    delete method;
    return 0;
}

SNESolver::SNESolver() :
    snes(nullptr),
    monitor_method(nullptr),
    compute_residual_method(nullptr),
    compute_jacobian_method(nullptr)
{
}

SNESolver::SNESolver(SNES snes) :
    snes(snes),
    monitor_method(nullptr),
    compute_residual_method(nullptr),
    compute_jacobian_method(nullptr)
{
}

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
SNESolver::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetFromOptions(this->snes));
}

void
SNESolver::set_function(Vector & r, PetscErrorCode (*callback)(SNES, Vec, Vec, void *), void * ctx)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetFunction(this->snes, r, callback, ctx));
}

void
SNESolver::set_jacobian(Matrix & J,
                        Matrix & Jp,
                        PetscErrorCode (*callback)(SNES, Vec, Mat, Mat, void *),
                        void * ctx)
{
    CALL_STACK_MSG();
    PETSC_CHECK(SNESSetJacobian(this->snes, J, Jp, callback, ctx));
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

SNESolver::operator SNES() const
{
    CALL_STACK_MSG();
    return this->snes;
}

} // namespace godzilla
