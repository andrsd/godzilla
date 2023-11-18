#include "godzilla/Godzilla.h"
#include "godzilla/FunctionAuxiliaryField.h"
#include "godzilla/CallStack.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

REGISTER_OBJECT(FunctionAuxiliaryField);

static PetscErrorCode
function_auxiliary_field(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
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
    _F_;
    return FunctionInterface::get_num_components();
}

PetscFunc *
FunctionAuxiliaryField::get_func() const
{
    _F_;
    return function_auxiliary_field;
}

void
FunctionAuxiliaryField::evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[])
{
    _F_;
    evaluate_func(dim, time, x, nc, u);
}

} // namespace godzilla
