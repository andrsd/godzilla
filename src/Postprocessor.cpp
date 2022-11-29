#include "Postprocessor.h"
#include "Problem.h"

namespace godzilla {

Parameters
Postprocessor::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<const Problem *>("_problem", nullptr);
    return params;
}

Postprocessor::Postprocessor(const Parameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<const Problem *>("_problem"))
{
}

} // namespace godzilla
