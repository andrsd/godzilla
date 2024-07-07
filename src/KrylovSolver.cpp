// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/KrylovSolver.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"

namespace godzilla {

ErrorCode
KrylovSolver::invoke_compute_operators_delegate(KSP, Mat A, Mat B, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Matrix & A, Matrix & B)> *>(ctx);
    Matrix mat_A(A);
    Matrix mat_B(B);
    method->invoke(mat_A, mat_B);
    return 0;
}

ErrorCode
KrylovSolver::invoke_compute_rhs_delegate(KSP, Vec b, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Vector & b)> *>(ctx);
    Vector vec_b(b);
    method->invoke(vec_b);
    return 0;
}

ErrorCode
KrylovSolver::invoke_monitor_delegate(KSP, Int it, Real rnorm, void * ctx)
{
    auto * method = static_cast<Delegate<void(Int it, Real rnorm)> *>(ctx);
    method->invoke(it, rnorm);
    return 0;
}

KrylovSolver::KrylovSolver() : ksp(nullptr) {}

KrylovSolver::KrylovSolver(KSP ksp) : ksp(ksp) {}

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
