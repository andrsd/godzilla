#include "Postprocessor.h"
#include "CallStack.h"
#include "Problem.h"

namespace godzilla {

Parameters
Postprocessor::valid_params()
{
    Parameters params = Object::valid_params();
    params.add_private_param<const Problem *>("_problem", nullptr);
    return params;
}

Postprocessor::Postprocessor(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<const Problem *>("_problem"))
{
}

Postprocessor::~Postprocessor() {}

} // namespace godzilla
