#include "Output.h"
#include "Problem.h"

namespace godzilla {

InputParameters
Output::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<Problem *>("_problem");
    return params;
}

Output::Output(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<Problem *>("_problem"))
{
    _F_;
}

} // namespace godzilla
