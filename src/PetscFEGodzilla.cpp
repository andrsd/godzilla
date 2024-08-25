// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FEProblemInterface.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/WeakForm.h"
#include "petsc/private/petscfeimpl.h"
#include "petscfe.h"

namespace godzilla {
namespace {

ErrorCode
integrate_residual(PetscDS ds,
                   PetscFormKey key,
                   Int ne,
                   PetscFEGeom * cgeom,
                   const Scalar coefficients[],
                   const Scalar coefficients_t[],
                   PetscDS ds_aux,
                   const Scalar coefficients_aux[],
                   Real t,
                   Scalar elem_vec[])
{
    CALL_STACK_MSG();
    WeakForm::Key wfk(key);
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, wfk.field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    fepi->integrate_residual(ds,
                             wfk,
                             ne,
                             cgeom,
                             coefficients,
                             coefficients_t,
                             ds_aux,
                             coefficients_aux,
                             t,
                             elem_vec);
    return 0;
}

ErrorCode
integrate_bd_residual(PetscDS ds,
                      PetscWeakForm /*wf*/,
                      PetscFormKey key,
                      Int ne,
                      PetscFEGeom * fgeom,
                      const Scalar coefficients[],
                      const Scalar coefficients_t[],
                      PetscDS ds_aux,
                      const Scalar coefficients_aux[],
                      Real t,
                      Scalar elem_vec[])
{
    CALL_STACK_MSG();
    WeakForm::Key wfk(key);
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, wfk.field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    fepi->integrate_bnd_residual(ds,
                                 wfk,
                                 ne,
                                 fgeom,
                                 coefficients,
                                 coefficients_t,
                                 ds_aux,
                                 coefficients_aux,
                                 t,
                                 elem_vec);
    return 0;
}

ErrorCode
integrate_jacobian(PetscDS ds,
                   PetscFEJacobianType jtype,
                   PetscFormKey key,
                   Int ne,
                   PetscFEGeom * cgeom,
                   const Scalar coefficients[],
                   const Scalar coefficients_t[],
                   PetscDS ds_aux,
                   const Scalar coefficients_aux[],
                   Real t,
                   Real u_tshift,
                   Scalar elem_mat[])
{
    CALL_STACK_MSG();
    WeakForm::Key wfk(key);
    auto field_i = wfk.jac.field_i;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, field_i, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    fepi->integrate_jacobian(ds,
                             jtype,
                             wfk,
                             ne,
                             cgeom,
                             coefficients,
                             coefficients_t,
                             ds_aux,
                             coefficients_aux,
                             t,
                             u_tshift,
                             elem_mat);
    return 0;
}

ErrorCode
integrate_bd_jacobian(PetscDS ds,
                      PetscWeakForm /*wf*/,
                      PetscFormKey key,
                      Int ne,
                      PetscFEGeom * fgeom,
                      const Scalar coefficients[],
                      const Scalar coefficients_t[],
                      PetscDS ds_aux,
                      const Scalar coefficients_aux[],
                      Real t,
                      Real u_tshift,
                      Scalar elem_mat[])
{
    CALL_STACK_MSG();
    WeakForm::Key wfk(key);
    auto field_i = wfk.jac.field_i;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, field_i, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    fepi->integrate_bnd_jacobian(ds,
                                 wfk,
                                 ne,
                                 fgeom,
                                 coefficients,
                                 coefficients_t,
                                 ds_aux,
                                 coefficients_aux,
                                 t,
                                 u_tshift,
                                 elem_mat);
    return 0;
}

} // namespace

namespace internal {

ErrorCode
create_lagrange_petscfe(MPI_Comm comm,
                        Int dim,
                        Int nc,
                        PetscBool is_simplex,
                        Int k,
                        Int qorder,
                        PetscFE * fem)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PetscFECreateLagrange(comm, dim, nc, is_simplex, k, qorder, fem));
    // replace PETSc functions with ours
    (*fem)->ops->integrate = nullptr;
    (*fem)->ops->integratebd = nullptr;
    (*fem)->ops->integrateresidual = integrate_residual;
    (*fem)->ops->integratebdresidual = integrate_bd_residual;
    (*fem)->ops->integratehybridresidual = nullptr;
    (*fem)->ops->integratejacobianaction = nullptr;
    (*fem)->ops->integratejacobian = integrate_jacobian;
    (*fem)->ops->integratebdjacobian = integrate_bd_jacobian;
    (*fem)->ops->integratehybridjacobian = nullptr;
    return 0;
}

} // namespace internal
} // namespace godzilla
