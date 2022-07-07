#include "Godzilla.h"
#include "Function.h"
#include "CallStack.h"

namespace godzilla {

Parameters
Function::parameters()
{
    Parameters params = Object::parameters();
    return params;
}

Function::Function(const Parameters & params) : Object(params)
{
    _F_;
}

} // namespace godzilla
