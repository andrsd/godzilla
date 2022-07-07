#include "Godzilla.h"
#include "Function.h"
#include "CallStack.h"

namespace godzilla {

Parameters
Function::valid_params()
{
    Parameters params = Object::valid_params();
    return params;
}

Function::Function(const Parameters & params) : Object(params)
{
    _F_;
}

} // namespace godzilla
