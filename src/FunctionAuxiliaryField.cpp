#include "Godzilla.h"
#include "FunctionAuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

registerObject(FunctionAuxiliaryField);

PetscErrorCode
__function_auxiliary_field(PetscInt dim,
                           PetscReal time,
                           const PetscReal x[],
                           PetscInt nc,
                           PetscScalar u[],
                           void * ctx)
{
    FunctionAuxiliaryField * func = (FunctionAuxiliaryField *) ctx;
    func->evaluate(dim, time, x, nc, u);
    return 0;
}

InputParameters
FunctionAuxiliaryField::validParams()
{
    InputParameters params = AuxiliaryField::validParams();
    params += FunctionInterface::validParams();
    return params;
}

FunctionAuxiliaryField::FunctionAuxiliaryField(const InputParameters & params) :
    AuxiliaryField(params),
    FunctionInterface(params)
{
    _F_;
}

void
FunctionAuxiliaryField::create()
{
    _F_;
    FunctionInterface::create();
}

PetscInt
FunctionAuxiliaryField::getNumComponents() const
{
    return this->num_comps;
}

void
FunctionAuxiliaryField::setUp(DM dm, DM dm_aux)
{
    _F_;
    PetscErrorCode ierr;
    PetscFunc * func[1] = { __function_auxiliary_field };
    void * ctxs[1] = { this };

    ierr = DMCreateLocalVector(dm_aux, &this->a);
    checkPetscError(ierr);

    ierr = DMProjectFunctionLocal(dm_aux,
                                  this->fepi.getTime(),
                                  func,
                                  ctxs,
                                  INSERT_ALL_VALUES,
                                  this->a);
    checkPetscError(ierr);

    ierr = DMSetAuxiliaryVec(dm, this->block, 0, 0, this->a);
    checkPetscError(ierr);
}

void
FunctionAuxiliaryField::evaluate(PetscInt dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 PetscInt nc,
                                 PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < nc; i++)
        u[i] = evaluateFunction(i, dim, time, x);
}

} // namespace godzilla
