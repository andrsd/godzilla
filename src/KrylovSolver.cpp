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

ErrorCode
KrylovSolver::invoke_convergence_test_delegate(KSP,
                                               Int it,
                                               Real rnorm,
                                               KSPConvergedReason * reason,
                                               void * ctx)
{
    auto * method = static_cast<Delegate<ConvergedReason(Int it, Real rnorm)> *>(ctx);
    auto conv_reason = method->invoke(it, rnorm);
    *reason = (KSPConvergedReason) conv_reason;
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

Matrix
KrylovSolver::get_operator() const
{
    CALL_STACK_MSG();
    Mat A;
    PETSC_CHECK(KSPGetOperators(this->ksp, &A, nullptr));
    return Matrix(A);
}

std::tuple<Matrix, Matrix>
KrylovSolver::get_operators() const
{
    CALL_STACK_MSG();
    Mat A, B;
    PETSC_CHECK(KSPGetOperators(this->ksp, &A, &B));
    return std::make_tuple(Matrix(A), Matrix(B));
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
KrylovSolver::get_tolerances(Real * rel_tol, Real * abs_tol, Real * div_tol, Int * max_its) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPGetTolerances(this->ksp, rel_tol, abs_tol, div_tol, max_its));
}

std::tuple<Real, Real, Real, Int>
KrylovSolver::get_tolerances() const
{
    CALL_STACK_MSG();
    Real rel_tol, abs_tol, div_tol;
    Int max_its;
    PETSC_CHECK(KSPGetTolerances(this->ksp, &rel_tol, &abs_tol, &div_tol, &max_its));
    return std::make_tuple(rel_tol, abs_tol, div_tol, max_its);
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

void
KrylovSolver::set_initial_guess_nonzero(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetInitialGuessNonzero(this->ksp, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
KrylovSolver::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetUp(this->ksp));
}

void
KrylovSolver::set_type(const char * type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetType(this->ksp, type));
}

void
KrylovSolver::set_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetType(this->ksp, type.c_str()));
}

void
KrylovSolver::set_pc_side(PCSide side)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetPCSide(this->ksp, side));
}

Vector
KrylovSolver::get_rhs() const
{
    CALL_STACK_MSG();
    Vec rhs;
    PETSC_CHECK(KSPGetRhs(this->ksp, &rhs));
    return Vector(rhs);
}

void
KrylovSolver::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPView(this->ksp, viewer));
}

std::string
KrylovSolver::converged_reason_str(ConvergedReason reason)
{
    CALL_STACK_MSG();
    if (reason == ConvergedReason::CONVERGED_ITERATING)
        return "iterating";
    else if (reason == ConvergedReason::CONVERGED_RTOL_NORMAL)
        return "relative tolerance";
    else if (reason == ConvergedReason::CONVERGED_ATOL_NORMAL)
        return "absolute tolerance";
    else if (reason == ConvergedReason::CONVERGED_RTOL)
        return "relative tolerance";
    else if (reason == ConvergedReason::CONVERGED_ATOL)
        return "absolute tolerance";
    else if (reason == ConvergedReason::CONVERGED_ITS)
        return "maximum iterations";
    else if (reason == ConvergedReason::CONVERGED_STEP_LENGTH)
        return "step length";
    else if (reason == ConvergedReason::CONVERGED_HAPPY_BREAKDOWN)
        return "happy breakdown";
    // Diverged reasons
    else if (reason == ConvergedReason::DIVERGED_NULL)
        return "null";
    else if (reason == ConvergedReason::DIVERGED_ITS)
        return "maximum iterations";
    else if (reason == ConvergedReason::DIVERGED_DTOL)
        return "divergent tolerance";
    else if (reason == ConvergedReason::DIVERGED_BREAKDOWN)
        return "breakdown";
    else if (reason == ConvergedReason::DIVERGED_BREAKDOWN_BICG)
        return "breakdown (BiCG)";
    else if (reason == ConvergedReason::DIVERGED_NONSYMMETRIC)
        return "non-symmetric matrix";
    else if (reason == ConvergedReason::DIVERGED_INDEFINITE_PC)
        return "indefinite preconditioner";
    else if (reason == ConvergedReason::DIVERGED_NANORINF)
        return "NaN or inf values";
    else if (reason == ConvergedReason::DIVERGED_INDEFINITE_MAT)
        return "indefinite matrix";
    else
        return "unknown";
}

} // namespace godzilla
