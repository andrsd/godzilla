#pragma once

#include "petsc.h"

namespace godzilla {

/// Function interface
///
/// This is used to interface with the PETSc API that accepts functions
class GFunctionInterface
{
public:
    GFunctionInterface();

    /// This is the API that we hand to PETSc for fields.
    void petscFieldFunc(PetscInt dim, PetscInt Nf, PetscInt NfAux,
        const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
        const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
        PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f[]);

    /// This is the API that we hand to PETSc for functions
    PetscErrorCode petscFunc(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

};

typedef
void (GFunctionInterface::*PetscFieldFuncPtr)(PetscInt dim, PetscInt Nf, PetscInt NfAux,
    const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
    const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
    PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f[]);

typedef
PetscErrorCode (GFunctionInterface::*PetscFuncPtr)(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

} // godzilla
