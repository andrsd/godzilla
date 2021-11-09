#include "GOutput.h"
#include "Problem.h"

namespace godzilla {

InputParameters
GOutput::validParams() {
    InputParameters params = Object::validParams();
    params.addPrivateParam<Problem *>("_Problem");
    return params;
}

GOutput::GOutput(const InputParameters & params) :
    Object(params),
    GPrintInterface(this),
    problem(*getParam<Problem *>("_Problem"))
{
    _F_;
}

} // godzilla
