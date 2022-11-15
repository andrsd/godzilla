#include <petsc/private/petscfeimpl.h>
#include "petscfe.h"
#include "FEProblemInterface.h"
#include "Error.h"

namespace godzilla {
namespace internal {

static PetscErrorCode
integrate(PetscDS ds,
          PetscInt field,
          PetscInt ne,
          PetscFEGeom * cgeom,
          const PetscScalar coefficients[],
          PetscDS ds_aux,
          const PetscScalar coefficients_aux[],
          PetscScalar integral[])
{
    PetscFunctionBegin;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err =
        fepi->integrate(ds, field, ne, cgeom, coefficients, ds_aux, coefficients_aux, integral);
    PetscFunctionReturn(err);
}

static PetscErrorCode
integrate_bd(PetscDS ds,
             PetscInt field,
             PetscBdPointFunc obj_func,
             PetscInt ne,
             PetscFEGeom * fgeom,
             const PetscScalar coefficients[],
             PetscDS ds_aux,
             const PetscScalar coefficients_aux[],
             PetscScalar integral[])
{
    PetscFunctionBegin;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, field, &ctx));
    auto * fepi = static_cast<godzilla::FEProblemInterface *>(ctx);
    PetscErrorCode err = fepi->integrate_bnd(ds,
                                             field,
                                             obj_func,
                                             ne,
                                             fgeom,
                                             coefficients,
                                             ds_aux,
                                             coefficients_aux,
                                             integral);
    PetscFunctionReturn(err);
}

static PetscErrorCode
integrate_residual(PetscDS ds,
                   PetscFormKey key,
                   PetscInt ne,
                   PetscFEGeom * cgeom,
                   const PetscScalar coefficients[],
                   const PetscScalar coefficients_t[],
                   PetscDS ds_aux,
                   const PetscScalar coefficients_aux[],
                   PetscReal t,
                   PetscScalar elem_vec[])
{
    PetscFunctionBegin;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, key.field, &ctx));
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
    PetscFunctionReturn(err);
}

static PetscErrorCode
integrate_bd_residual(PetscDS ds,
                      PetscWeakForm /*wf*/,
                      PetscFormKey key,
                      PetscInt ne,
                      PetscFEGeom * fgeom,
                      const PetscScalar coefficients[],
                      const PetscScalar coefficients_t[],
                      PetscDS ds_aux,
                      const PetscScalar coefficients_aux[],
                      PetscReal t,
                      PetscScalar elem_vec[])
{
    PetscFunctionBegin;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, key.field, &ctx));
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
    PetscFunctionReturn(err);
}

PetscErrorCode
integrate_jacobian(PetscDS ds,
                   PetscFEJacobianType jtype,
                   PetscFormKey key,
                   PetscInt ne,
                   PetscFEGeom * cgeom,
                   const PetscScalar coefficients[],
                   const PetscScalar coefficients_t[],
                   PetscDS ds_aux,
                   const PetscScalar coefficients_aux[],
                   PetscReal t,
                   PetscReal u_tshift,
                   PetscScalar elem_mat[])
{
    PetscFunctionBegin;
    PetscInt n_fields;
    PetscCall(PetscDSGetNumFields(ds, &n_fields));
    PetscInt field_i = key.field / n_fields;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, field_i, &ctx));
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
    PetscFunctionReturn(err);
}

static PetscErrorCode
integrate_bd_jacobian(PetscDS ds,
                      PetscWeakForm /*wf*/,
                      PetscFormKey key,
                      PetscInt ne,
                      PetscFEGeom * fgeom,
                      const PetscScalar coefficients[],
                      const PetscScalar coefficients_t[],
                      PetscDS ds_aux,
                      const PetscScalar coefficients_aux[],
                      PetscReal t,
                      PetscReal u_tshift,
                      PetscScalar elem_mat[])
{
    PetscFunctionBegin;
    PetscInt n_fields;
    PetscCall(PetscDSGetNumFields(ds, &n_fields));
    PetscInt field_i = key.field / n_fields;
    void * ctx;
    PetscCall(PetscDSGetContext(ds, field_i, &ctx));
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
    PetscFunctionReturn(err);
}

PetscErrorCode
create_lagrange_petscfe(MPI_Comm comm,
                        PetscInt dim,
                        PetscInt nc,
                        PetscBool is_simplex,
                        PetscInt k,
                        PetscInt qorder,
                        PetscFE * fem)
{
    PetscFunctionBegin;
    PetscCall(PetscFECreateLagrange(comm, dim, nc, is_simplex, k, qorder, fem));
    // replace PETSc functions with ours
    (*fem)->ops->integrate = integrate;
    (*fem)->ops->integratebd = integrate_bd;
    (*fem)->ops->integrateresidual = integrate_residual;
    (*fem)->ops->integratebdresidual = integrate_bd_residual;
    (*fem)->ops->integratehybridresidual = nullptr;
    (*fem)->ops->integratejacobianaction = nullptr;
    (*fem)->ops->integratejacobian = integrate_jacobian;
    (*fem)->ops->integratebdjacobian = integrate_bd_jacobian;
    (*fem)->ops->integratehybridjacobian = nullptr;
    PetscFunctionReturn(0);
}

} // namespace internal
} // namespace godzilla
