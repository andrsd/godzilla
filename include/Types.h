#pragma once

#include "petsc.h"

namespace godzilla {

typedef PetscInt Int;
typedef PetscReal Real;
typedef PetscScalar Scalar;

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
typedef void PetscFieldFunc(Int dim,
                            Int Nf,
                            Int NfAux,
                            const Int uOff[],
                            const Int uOff_x[],
                            const Scalar u[],
                            const Scalar u_t[],
                            const Scalar u_x[],
                            const Int aOff[],
                            const Int aOff_x[],
                            const Scalar a[],
                            const Scalar a_t[],
                            const Scalar a_x[],
                            Real t,
                            const Real x[],
                            Int numConstants,
                            const Scalar constants[],
                            Scalar f[]);

typedef PetscErrorCode
PetscFunc(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx);

typedef PetscErrorCode PetscNaturalRiemannBCFunc(Real time,
                                                 const Real * c,
                                                 const Real * n,
                                                 const Scalar * xI,
                                                 Scalar * xG,
                                                 void * ctx);

} // namespace godzilla
