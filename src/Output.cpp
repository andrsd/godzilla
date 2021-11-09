#include "Output.h"
#include "Problem.h"

namespace godzilla {

InputParameters
Output::validParams() {
    InputParameters params = Object::validParams();
    params.addPrivateParam<Problem *>("_problem");
    return params;
}

Output::Output(const InputParameters & params) :
    Object(params),
    PrintInterface(this),
    problem(*getParam<Problem *>("_problem"))
{
    _F_;
}

} // godzilla
