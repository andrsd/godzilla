#include "Postprocessor.h"
#include "CallStack.h"
#include "Problem.h"

namespace godzilla {

InputParameters
Postprocessor::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<const Problem *>("_problem", nullptr);
    return params;
}

Postprocessor::Postprocessor(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(*getParam<Problem *>("_problem"))
{
}

Postprocessor::~Postprocessor() {}

} // namespace godzilla
