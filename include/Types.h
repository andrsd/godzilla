#pragma once

#include "petsc.h"

namespace godzilla {

/// This is the API that we hand to PETSc for fields.
///
/// @param dim The spatial dimension
/// @param Nf The number of input fields
/// @param NfAux The number of input auxiliary fields
/// @param uOff The offset of each field in u[]
/// @param uOff_x The offset of each field in u_x[]
/// @param u The field values at this point in space
/// @param u_t The field time derivative at this point in space (or NULL)
/// @param u_x The field derivatives at this point in space
/// @param aOff The offset of each auxiliary field in u[]
/// @param aOff_x The offset of each auxiliary field in u_x[]
/// @param a The auxiliary field values at this point in space
/// @param a_t The auxiliary field time derivative at this point in space (or NULL)
/// @param a_x The auxiliary field derivatives at this point in space
/// @param t The current time
/// @param x The coordinates of this point
/// @param numConstants The number of constants
/// @param constants The value of each constant
/// @param f The value of the function at this point in space
typedef void PetscFieldFunc(PetscInt dim,
                            PetscInt Nf,
                            PetscInt NfAux,
                            const PetscInt uOff[],
                            const PetscInt uOff_x[],
                            const PetscScalar u[],
                            const PetscScalar u_t[],
                            const PetscScalar u_x[],
                            const PetscInt aOff[],
                            const PetscInt aOff_x[],
                            const PetscScalar a[],
                            const PetscScalar a_t[],
                            const PetscScalar a_x[],
                            PetscReal t,
                            const PetscReal x[],
                            PetscInt numConstants,
                            const PetscScalar constants[],
                            PetscScalar f[]);

typedef PetscErrorCode PetscFunc(PetscInt dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 PetscInt Nc,
                                 PetscScalar u[],
                                 void * ctx);

typedef void PetscFEResidualFunc(PetscInt dim,
                                 PetscInt Nf,
                                 PetscInt NfAux,
                                 const PetscInt uOff[],
                                 const PetscInt uOff_x[],
                                 const PetscScalar u[],
                                 const PetscScalar u_t[],
                                 const PetscScalar u_x[],
                                 const PetscInt aOff[],
                                 const PetscInt aOff_x[],
                                 const PetscScalar a[],
                                 const PetscScalar a_t[],
                                 const PetscScalar a_x[],
                                 PetscReal t,
                                 const PetscReal x[],
                                 PetscInt numConstants,
                                 const PetscScalar constants[],
                                 PetscScalar f0[]);

typedef void PetscFEJacobianFunc(PetscInt dim,
                                 PetscInt Nf,
                                 PetscInt NfAux,
                                 const PetscInt uOff[],
                                 const PetscInt uOff_x[],
                                 const PetscScalar u[],
                                 const PetscScalar u_t[],
                                 const PetscScalar u_x[],
                                 const PetscInt aOff[],
                                 const PetscInt aOff_x[],
                                 const PetscScalar a[],
                                 const PetscScalar a_t[],
                                 const PetscScalar a_x[],
                                 PetscReal t,
                                 PetscReal u_tShift,
                                 const PetscReal x[],
                                 PetscInt numConstants,
                                 const PetscScalar constants[],
                                 PetscScalar g3[]);

typedef void PetscFEBndResidualFunc(PetscInt dim,
                                    PetscInt nf,
                                    PetscInt nf_aux,
                                    const PetscInt u_off[],
                                    const PetscInt u_off_x[],
                                    const PetscScalar u[],
                                    const PetscScalar u_t[],
                                    const PetscScalar u_x[],
                                    const PetscInt a_off[],
                                    const PetscInt a_off_x[],
                                    const PetscScalar a[],
                                    const PetscScalar a_t[],
                                    const PetscScalar a_x[],
                                    PetscReal t,
                                    const PetscReal x[],
                                    const PetscReal n[],
                                    PetscInt num_constants,
                                    const PetscScalar constants[],
                                    PetscScalar f[]);

typedef void PetscFEBndJacobianFunc(PetscInt dim,
                                    PetscInt nf,
                                    PetscInt nf_aux,
                                    const PetscInt u_off[],
                                    const PetscInt u_off_x[],
                                    const PetscScalar u[],
                                    const PetscScalar u_t[],
                                    const PetscScalar u_x[],
                                    const PetscInt a_off[],
                                    const PetscInt a_off_x[],
                                    const PetscScalar a[],
                                    const PetscScalar a_t[],
                                    const PetscScalar a_x[],
                                    PetscReal t,
                                    PetscReal u_t_shift,
                                    const PetscReal x[],
                                    const PetscReal n[],
                                    PetscInt num_constants,
                                    const PetscScalar constants[],
                                    PetscScalar g[]);

} // namespace godzilla
