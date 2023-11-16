#include <petsc/private/petscfeimpl.h>
#include "petscfe.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"

namespace godzilla {
namespace internal {

static PetscErrorCode
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
    _F_;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, key.field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err = fepi->integrate_residual(ds,
                                                  key,
                                                  ne,
                                                  cgeom,
                                                  coefficients,
                                                  coefficients_t,
                                                  ds_aux,
                                                  coefficients_aux,
                                                  t,
                                                  elem_vec);
    return err;
}

static PetscErrorCode
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
    _F_;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, key.field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err = fepi->integrate_bnd_residual(ds,
                                                      key,
                                                      ne,
                                                      fgeom,
                                                      coefficients,
                                                      coefficients_t,
                                                      ds_aux,
                                                      coefficients_aux,
                                                      t,
                                                      elem_vec);
    return err;
}

PetscErrorCode
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
    _F_;
    Int n_fields;
    PETSC_CHECK(PetscDSGetNumFields(ds, &n_fields));
    Int field_i = key.field / n_fields;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, field_i, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err = fepi->integrate_jacobian(ds,
                                                  jtype,
                                                  key,
                                                  ne,
                                                  cgeom,
                                                  coefficients,
                                                  coefficients_t,
                                                  ds_aux,
                                                  coefficients_aux,
                                                  t,
                                                  u_tshift,
                                                  elem_mat);
    return err;
}

static PetscErrorCode
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
    _F_;
    Int n_fields;
    PETSC_CHECK(PetscDSGetNumFields(ds, &n_fields));
    Int field_i = key.field / n_fields;
    void * ctx;
    PETSC_CHECK(PetscDSGetContext(ds, field_i, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err = fepi->integrate_bnd_jacobian(ds,
                                                      key,
                                                      ne,
                                                      fgeom,
                                                      coefficients,
                                                      coefficients_t,
                                                      ds_aux,
                                                      coefficients_aux,
                                                      t,
                                                      u_tshift,
                                                      elem_mat);
    return err;
}

PetscErrorCode
create_lagrange_petscfe(MPI_Comm comm,
                        Int dim,
                        Int nc,
                        PetscBool is_simplex,
                        Int k,
                        Int qorder,
                        PetscFE * fem)
{
    _F_;
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
