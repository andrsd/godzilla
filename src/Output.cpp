#include "Output.h"
#include "CallStack.h"
#include "Problem.h"
#include <assert.h>

namespace godzilla {

InputParameters
Output::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<const Problem *>("_problem");
    return params;
}

Output::Output(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(get_param<const Problem *>("_problem"))
{
    _F_;
    assert(this->problem != nullptr);
}

} // namespace godzilla
