#include "Godzilla.h"
#include "Function.h"
#include "CallStack.h"

namespace godzilla {

InputParameters
Function::valid_params()
{
    InputParameters params = Object::valid_params();
    return params;
}

Function::Function(const InputParameters & params) : Object(params)
{
    _F_;
}

} // namespace godzilla
