#include "AuxiliaryField.h"
#include "CallStack.h"
#include "FEProblemInterface.h"

namespace godzilla {

InputParameters
AuxiliaryField::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<const FEProblemInterface *>("_fepi", nullptr);
    return params;
}

AuxiliaryField::AuxiliaryField(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    fepi(*get_param<FEProblemInterface *>("_fepi")),
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
AuxiliaryField::get_field_id() const
{
    _F_;
    return this->fepi.get_aux_field_id(this->get_name());
}

} // namespace godzilla
