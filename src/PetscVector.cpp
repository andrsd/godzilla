#include "PetscVector.h"
#include "CallStack.h"
#include "Error.h"
#include "petsc.h"

namespace godzilla {

PetscVector::PetscVector(Vec vec) : vec(vec)
{
    _F_;
}

PetscVector::~PetscVector()
{
    _F_;
}

void
PetscVector::finish()
{
    _F_;
    PetscErrorCode ierr;

    ierr = VecAssemblyBegin(this->vec);
    checkPetscError(ierr);
    ierr = VecAssemblyEnd(this->vec);
    checkPetscError(ierr);
}

Scalar
PetscVector::get(PetscInt idx)
{
    _F_;
    PetscErrorCode ierr;
    Scalar y = 0;
    ierr = VecGetValues(this->vec, 1, &idx, &y);
    checkPetscError(ierr);
    return y;
}

void
PetscVector::zero()
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecZeroEntries(this->vec);
    checkPetscError(ierr);
}

void
PetscVector::set(PetscInt idx, PetscScalar y)
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSetValue(vec, idx, y, INSERT_VALUES);
    checkPetscError(ierr);
}

void
PetscVector::add(PetscInt idx, PetscScalar y)
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSetValue(this->vec, idx, (PetscScalar) y, ADD_VALUES);
    checkPetscError(ierr);
}

void
PetscVector::add(PetscInt n, const PetscInt * idx, const PetscScalar * y)
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecSetValues(this->vec, n, idx, y, ADD_VALUES);
    checkPetscError(ierr);
}

void
PetscVector::add(const DenseVector<PetscScalar> & v, PetscInt *idx)
{
    _F_;
    PetscErrorCode ierr;
    for (uint i = 0; i < v.rows(); i++) {
        ierr = VecSetValue(this->vec, idx[i], v[i], ADD_VALUES);
        checkPetscError(ierr);
    }
}


} // namespace godzilla
