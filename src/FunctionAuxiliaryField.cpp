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
FunctionAuxiliaryField::valid_params()
{
    InputParameters params = AuxiliaryField::valid_params();
    params += FunctionInterface::valid_params();
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
FunctionAuxiliaryField::get_num_components() const
{
    return this->num_comps;
}

void
FunctionAuxiliaryField::set_up(DM dm, DM dm_aux)
{
    _F_;
    PetscErrorCode ierr;
    PetscFunc * func[1] = { __function_auxiliary_field };
    void * ctxs[1] = { this };

    ierr = DMCreateLocalVector(dm_aux, &this->a);
    check_petsc_error(ierr);

    ierr = DMProjectFunctionLocal(dm_aux,
                                  this->fepi.get_time(),
                                  func,
                                  ctxs,
                                  INSERT_ALL_VALUES,
                                  this->a);
    check_petsc_error(ierr);

    ierr = DMSetAuxiliaryVec(dm, this->block, 0, 0, this->a);
    check_petsc_error(ierr);
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
        u[i] = FunctionInterface::evaluate(i, dim, time, x);
}

} // namespace godzilla
