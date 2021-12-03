#include "AuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

InputParameters
AuxiliaryField::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<const FEProblemInterface *>("_fepi", nullptr);
    return params;
}

AuxiliaryField::AuxiliaryField(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    fepi(*getParam<FEProblemInterface *>("_fepi")),
    a(nullptr),
    // TODO: set to the block where this aux field lives (nullptr => everywhere)
    block(nullptr)
{
}

AuxiliaryField::~AuxiliaryField()
{
    _F_;
    if (this->a)
        VecDestroy(&this->a);
}

PetscInt
AuxiliaryField::getFieldId() const
{
    _F_;
    return this->fepi.getAuxFieldId(this->getName());
}

} // namespace godzilla
