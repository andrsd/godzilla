// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/KrylovSolver.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Matrix.h"
#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Vector.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Convert.h"

namespace godzilla {

ErrorCode
KrylovSolver::invoke_compute_operators_delegate(KSP, Mat A, Mat B, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Matrix & A, Matrix & B)> *>(ctx);
    Matrix mat_A(A);
    mat_A.inc_reference();
    Matrix mat_B(B);
    mat_B.inc_reference();
    method->invoke(mat_A, mat_B);
    return 0;
}

ErrorCode
KrylovSolver::invoke_compute_rhs_delegate(KSP, Vec b, void * ctx)
{
    CALL_STACK_MSG();
    auto * method = static_cast<Delegate<void(Vector & b)> *>(ctx);
    Vector vec_b(b);
    vec_b.inc_reference();
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

KrylovSolver::KrylovSolver() : PetscObjectWrapper(nullptr) {}

KrylovSolver::KrylovSolver(KSP ksp) : PetscObjectWrapper(ksp) {}

void
KrylovSolver::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPCreate(comm, &this->obj));
}

void
KrylovSolver::destroy()
{
    CALL_STACK_MSG();
}

void
KrylovSolver::set_dm(DM dm)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetDM(this->obj, dm));
}

void
KrylovSolver::set_operators(const Matrix & A, const Matrix & B) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetOperators(this->obj, A, B));
}

void
KrylovSolver::set_operator(const Matrix & A) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetOperators(this->obj, A, A));
}

Matrix
KrylovSolver::get_operator() const
{
    CALL_STACK_MSG();
    Matrix A;
    PETSC_CHECK(KSPGetOperators(this->obj, A, nullptr));
    A.inc_reference();
    return A;
}

std::tuple<Matrix, Matrix>
KrylovSolver::get_operators() const
{
    CALL_STACK_MSG();
    Matrix A, B;
    PETSC_CHECK(KSPGetOperators(this->obj, A, B));
    A.inc_reference();
    B.inc_reference();
    return { A, B };
}

void
KrylovSolver::set_from_options()
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetFromOptions(this->obj));
}

void
KrylovSolver::set_tolerances(Real rel_tol, Real abs_tol, Real div_tol, Int max_its)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetTolerances(this->obj, rel_tol, abs_tol, div_tol, max_its));
}

void
KrylovSolver::get_tolerances(Real * rel_tol, Real * abs_tol, Real * div_tol, Int * max_its) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPGetTolerances(this->obj, rel_tol, abs_tol, div_tol, max_its));
}

std::tuple<Real, Real, Real, Int>
KrylovSolver::get_tolerances() const
{
    CALL_STACK_MSG();
    Real rel_tol, abs_tol, div_tol;
    Int max_its;
    PETSC_CHECK(KSPGetTolerances(this->obj, &rel_tol, &abs_tol, &div_tol, &max_its));
    return std::make_tuple(rel_tol, abs_tol, div_tol, max_its);
}

void
KrylovSolver::solve(Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSolve(this->obj, x, x));
}

void
KrylovSolver::solve(const Vector & b, Vector & x) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSolve(this->obj, b, x));
}

KrylovSolver::ConvergedReason
KrylovSolver::get_converged_reason() const
{
    CALL_STACK_MSG();
    KSPConvergedReason reason;
    PETSC_CHECK(KSPGetConvergedReason(this->obj, &reason));
    return static_cast<ConvergedReason>(reason);
}

PC
KrylovSolver::get_pc() const
{
    CALL_STACK_MSG();
    PC pc;
    PETSC_CHECK(KSPGetPC(this->obj, &pc));
    return pc;
}

void
KrylovSolver::set_initial_guess_nonzero(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetInitialGuessNonzero(this->obj, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
KrylovSolver::set_up()
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetUp(this->obj));
}

void
KrylovSolver::set_type(const char * type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetType(this->obj, type));
}

void
KrylovSolver::set_type(String type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetType(this->obj, type.c_str()));
}

void
KrylovSolver::set_pc_side(PCSide side)
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPSetPCSide(this->obj, side));
}

Vector
KrylovSolver::get_rhs() const
{
    CALL_STACK_MSG();
    Vector rhs;
    PETSC_CHECK(KSPGetRhs(this->obj, rhs));
    rhs.inc_reference();
    return rhs;
}

void
KrylovSolver::view(PetscViewer viewer) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(KSPView(this->obj, viewer));
}

void
print_converged_reason(PrintInterface & pi, KrylovSolver::ConvergedReason reason)
{
    CALL_STACK_MSG();
    if (reason > 0)
        pi.lprintln(8, Terminal::green, "Converged: {}", conv::to_str(reason));
    else
        pi.lprintln(8, Terminal::red, "Not converged: {}", conv::to_str(reason));
}

} // namespace godzilla
