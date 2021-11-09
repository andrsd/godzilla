#include "Output.h"
#include "Problem.h"

namespace godzilla {

InputParameters
Output::validParams() {
    InputParameters params = Object::validParams();
    params.addPrivateParam<Problem *>("_Problem");
    return params;
}

Output::Output(const InputParameters & params) :
    Object(params),
    GPrintInterface(this),
    problem(*getParam<Problem *>("_Problem"))
{
    _F_;
}

} // godzilla
