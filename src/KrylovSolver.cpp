// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/KrylovSolver.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"

namespace godzilla {

PetscErrorCode
KrylovSolver::compute_operators(KSP, Mat A, Mat B, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<KSPComputeOperatorsMethodAbstract *>(ctx);
    Matrix mat_A(A);
    Matrix mat_B(B);
    return method->invoke(mat_A, mat_B);
}

PetscErrorCode
KrylovSolver::compute_rhs(KSP, Vec b, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<KSPComputeRhsMethodAbstract *>(ctx);
    Vector vec_b(b);
    return method->invoke(vec_b);
}

PetscErrorCode
KrylovSolver::monitor(KSP, Int it, Real rnorm, void * ctx)
{
    auto * method = static_cast<KSPMonitorMethodAbstract *>(ctx);
    return method->invoke(it, rnorm);
}

PetscErrorCode
KrylovSolver::monitor_destroy(void ** ctx)
{
    auto * method = static_cast<KSPMonitorMethodAbstract *>(*ctx);
    delete method;
    return 0;
}

KrylovSolver::KrylovSolver() :
    ksp(nullptr),
    monitor_method(nullptr),
    compute_rhs_method(nullptr),
    compute_operators_method(nullptr)
{
}

KrylovSolver::KrylovSolver(KSP ksp) :
    ksp(ksp),
    monitor_method(nullptr),
    compute_rhs_method(nullptr),
    compute_operators_method(nullptr)
{
}

void
KrylovSolver::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPCreate(comm, &this->ksp));
}

void
KrylovSolver::destroy()
{
    CALL_STACK_MSG();
    if (this->ksp)
        KSPDestroy(&this->ksp);
}

void
KrylovSolver::set_dm(DM dm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetDM(this->ksp, dm));
}

void
KrylovSolver::set_operators(const Matrix & A, const Matrix & B) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetOperators(this->ksp, A, B));
}

void
KrylovSolver::set_operator(const Matrix & A) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetOperators(this->ksp, A, A));
}

void
KrylovSolver::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetFromOptions(this->ksp));
}

void
KrylovSolver::set_tolerances(Real rel_tol, Real abs_tol, Real div_tol, Int max_its)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetTolerances(this->ksp, rel_tol, abs_tol, div_tol, max_its));
}

void
KrylovSolver::set_compute_rhs(PetscErrorCode (*func)(KSP ksp, Vec b, void * ctx), void * ctx)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetComputeRHS(this->ksp, func, ctx));
}

void
KrylovSolver::set_compute_operators(PetscErrorCode (*func)(KSP ksp, Mat A, Mat B, void * ctx),
                                    void * ctx)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetComputeOperators(this->ksp, func, ctx));
}

void
KrylovSolver::monitor_set(
    PetscErrorCode (*monitor)(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx),
    void * ctx,
    PetscErrorCode (*monitordestroy)(void ** ctx))
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPMonitorSet(this->ksp, monitor, ctx, monitordestroy));
}

void
KrylovSolver::solve(Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSolve(this->ksp, x, x));
}

void
KrylovSolver::solve(const Vector & b, Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSolve(this->ksp, b, x));
}

KrylovSolver::ConvergedReason
KrylovSolver::get_converged_reason() const
{
    CALL_STACK_MSG();
    KSPConvergedReason reason;
    PETSC_CHECK(KSPGetConvergedReason(this->ksp, &reason));
    return static_cast<ConvergedReason>(reason);
}

PC
KrylovSolver::get_pc() const
{
    CALL_STACK_MSG();
    PC pc;
    PETSC_CHECK(KSPGetPC(this->ksp, &pc));
    return pc;
}

KrylovSolver::operator KSP() const
{
    CALL_STACK_MSG();
    return this->ksp;
}

} // namespace godzilla
