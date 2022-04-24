#pragma once

#include "petsc.h"
#include "Common.h"

namespace godzilla {

/// Wrapper around PETSc vector
///
class PetscVector {
public:
    PetscVector(Vec vec);
    virtual ~PetscVector();

    virtual void finish();
    virtual Scalar get(PetscInt idx);
    virtual void zero();
    virtual void set(PetscInt idx, PetscScalar y);
    virtual void add(PetscInt idx, PetscScalar y);
    virtual void add(PetscInt n, const PetscInt * idx, const PetscScalar * y);

protected:
    Vec vec;
};

} // namespace godzilla
