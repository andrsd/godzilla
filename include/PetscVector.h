#pragma once

#include "petsc.h"
#include "DenseVector.h"
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
    virtual void set(PetscInt idx, Scalar y);
    virtual void add(PetscInt idx, Scalar y);
    virtual void add(PetscInt n, const PetscInt * idx, const Scalar * y);
    virtual void add(const DenseVector<Scalar> & mat, PetscInt *idx);

protected:
    Vec vec;
};

} // namespace godzilla
