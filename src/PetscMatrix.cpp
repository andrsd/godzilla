#include "PetscMatrix.h"
#include "CallStack.h"
#include "Error.h"
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
    PetscScalar v = 0.0;
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
PetscMatrix::add(PetscInt m, PetscInt n, PetscScalar v)
{
    _F_;
    PetscErrorCode ierr;
    ierr = MatSetValue(this->mat, m, n, v, ADD_VALUES);
    checkPetscError(ierr);
}

} // namespace godzilla
