#include "Godzilla.h"
#include "Function.h"

namespace godzilla {

InputParameters
Function::validParams()
{
    InputParameters params = Object::validParams();
    return params;
}

Function::Function(const InputParameters & params) : Object(params)
{
    _F_;
}

} // namespace godzilla
