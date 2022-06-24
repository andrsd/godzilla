#include "TimeSteppingAdaptor.h"
#include "CallStack.h"
#include "TransientProblemInterface.h"

namespace godzilla {

InputParameters
TimeSteppingAdaptor::valid_params()
{
    InputParameters params = Object::valid_params();
    params.add_private_param<const Problem *>("_problem", nullptr);
    params.add_private_param<const TransientProblemInterface *>("_tpi", nullptr);
    return params;
}

TimeSteppingAdaptor::TimeSteppingAdaptor(const InputParameters & params) :
    Object(params),
    problem(get_param<const Problem *>("_problem")),
    tpi(get_param<const TransientProblemInterface *>("_tpi"))
{
    _F_;
}

} // namespace godzilla
