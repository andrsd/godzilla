#include "PetscMatrix.h"
#include "CallStack.h"
#include "Error.h"
#include "Space.h"
#include "petsc.h"

namespace godzilla {

PetscMatrix::PetscMatrix(Mat mat) : mat(mat)
{
    _F_;
}

PetscMatrix::~PetscMatrix()
{
    _F_;
}

void
PetscMatrix::finish()
{
    _F_;
    MatAssemblyBegin(this->mat, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(this->mat, MAT_FINAL_ASSEMBLY);
}

Scalar
PetscMatrix::get(PetscInt m, PetscInt n)
{
    _F_;
    PetscErrorCode ierr;
    Scalar v = 0.0;
    ierr = MatGetValue(this->mat, m, n, &v);
    checkPetscError(ierr);
    return v;
}

void
PetscMatrix::zero()
{
    _F_;
    PetscErrorCode ierr;
    ierr = MatZeroEntries(this->mat);
    checkPetscError(ierr);
}

void
PetscMatrix::add(PetscInt m, PetscInt n, Scalar v)
{
    _F_;
    PetscErrorCode ierr;
    // ignore Dirichlet DoF
    // TODO: Make it so that we never pass dirichlet DoFs here, so we can remove this if-statement
    if (m != Space::DIRICHLET_DOF && n != Space::DIRICHLET_DOF) {
        ierr = MatSetValue(this->mat, m, n, v, ADD_VALUES);
        checkPetscError(ierr);
    }
}

void
PetscMatrix::add(const DenseMatrix<Scalar> & mat,
                 PetscInt * rows,
                 PetscInt * cols)
{
    _F_;
    for (uint i = 0; i < mat.rows(); i++)
        for (int j = 0; j < mat.cols(); j++)
            add(rows[i], cols[j], mat[i][j]);
}

} // namespace godzilla
