#include "Postprocessor.h"
#include "CallStack.h"
#include "Problem.h"

namespace godzilla {

InputParameters
Postprocessor::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<const Problem *>("_problem", nullptr);
    return params;
}

Postprocessor::Postprocessor(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(*get_param<Problem *>("_problem"))
{
}

Postprocessor::~Postprocessor() {}

} // namespace godzilla
