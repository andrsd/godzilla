#pragma once

#include "Common.h"
#include "DenseMatrix.h"
#include "petsc.h"

namespace godzilla {

/// Wrapper around PETSc matrix
///
class PetscMatrix {
public:
    PetscMatrix(Mat mat);
    virtual ~PetscMatrix();

    virtual void finish();
    virtual Scalar get(PetscInt m, PetscInt n);
    virtual void zero();
    virtual void add(PetscInt m, PetscInt n, Scalar v);
    virtual void add(const DenseMatrix<Scalar> & mat, PetscInt * rows, PetscInt * cols);

protected:
    Mat mat;
};

} // namespace godzilla
