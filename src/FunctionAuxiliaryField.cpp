#include "Godzilla.h"
#include "FunctionAuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

REGISTER_OBJECT(FunctionAuxiliaryField);

static PetscErrorCode
function_auxiliary_field(Int dim,
                         PetscReal time,
                         const PetscReal x[],
                         Int nc,
                         PetscScalar u[],
                         void * ctx)
{
    auto * func = static_cast<FunctionAuxiliaryField *>(ctx);
    func->evaluate(dim, time, x, nc, u);
    return 0;
}

Parameters
FunctionAuxiliaryField::parameters()
{
    Parameters params = AuxiliaryField::parameters();
    params += FunctionInterface::parameters();
    return params;
}

FunctionAuxiliaryField::FunctionAuxiliaryField(const Parameters & params) :
    AuxiliaryField(params),
    FunctionInterface(params)
{
    _F_;
}

void
FunctionAuxiliaryField::create()
{
    _F_;
    AuxiliaryField::create();
    FunctionInterface::create();
}

Int
FunctionAuxiliaryField::get_num_components() const
{
    return this->num_comps;
}

PetscFunc *
FunctionAuxiliaryField::get_func() const
{
    return function_auxiliary_field;
}

void
FunctionAuxiliaryField::evaluate(Int dim,
                                 PetscReal time,
                                 const PetscReal x[],
                                 Int nc,
                                 PetscScalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
