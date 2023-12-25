// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/KrylovSolver.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"

namespace godzilla {

KrylovSolver::KrylovSolver() : ksp(nullptr) {}

KrylovSolver::KrylovSolver(KSP ksp) : ksp(ksp) {}

void
KrylovSolver::create(MPI_Comm comm)
{
    _F_;
    PETSC_CHECK(KSPCreate(comm, &this->ksp));
}

void
KrylovSolver::destroy()
{
    _F_;
    if (this->ksp)
        KSPDestroy(&this->ksp);
}

void
KrylovSolver::set_dm(DM dm)
{
    _F_;
    PETSC_CHECK(KSPSetDM(this->ksp, dm));
}

void
KrylovSolver::set_operators(const Matrix & A, const Matrix & B) const
{
    _F_;
    PETSC_CHECK(KSPSetOperators(this->ksp, A, B));
}

void
KrylovSolver::set_operator(const Matrix & A) const
{
    _F_;
    PETSC_CHECK(KSPSetOperators(this->ksp, A, A));
}

void
KrylovSolver::set_from_options()
{
    _F_;
    PETSC_CHECK(KSPSetFromOptions(this->ksp));
}

void
KrylovSolver::set_tolerances(Real rel_tol, Real abs_tol, Real div_tol, Int max_its)
{
    _F_;
    PETSC_CHECK(KSPSetTolerances(this->ksp, rel_tol, abs_tol, div_tol, max_its));
}

void
KrylovSolver::set_compute_rhs(PetscErrorCode (*func)(KSP ksp, Vec b, void * ctx), void * ctx)
{
    _F_;
    PETSC_CHECK(KSPSetComputeRHS(this->ksp, func, ctx));
}

void
KrylovSolver::set_compute_operators(PetscErrorCode (*func)(KSP ksp, Mat A, Mat B, void * ctx),
                                    void * ctx)
{
    _F_;
    PETSC_CHECK(KSPSetComputeOperators(this->ksp, func, ctx));
}

void
KrylovSolver::monitor_set(
    PetscErrorCode (*monitor)(KSP ksp, PetscInt it, PetscReal rnorm, void * ctx),
    void * ctx,
    PetscErrorCode (*monitordestroy)(void ** ctx))
{
    _F_;
    PETSC_CHECK(KSPMonitorSet(this->ksp, monitor, ctx, monitordestroy));
}

void
KrylovSolver::solve(Vector & x) const
{
    _F_;
    PETSC_CHECK(KSPSolve(this->ksp, x, x));
}

void
KrylovSolver::solve(const Vector & b, Vector & x) const
{
    _F_;
    PETSC_CHECK(KSPSolve(this->ksp, b, x));
}

KrylovSolver::ConvergedReason
KrylovSolver::get_converged_reason() const
{
    _F_;
    KSPConvergedReason reason;
    PETSC_CHECK(KSPGetConvergedReason(this->ksp, &reason));
    return static_cast<ConvergedReason>(reason);
}

PC
KrylovSolver::get_pc() const
{
    _F_;
    PC pc;
    PETSC_CHECK(KSPGetPC(this->ksp, &pc));
    return pc;
}

KrylovSolver::operator KSP() const
{
    _F_;
    return this->ksp;
}

} // namespace godzilla
