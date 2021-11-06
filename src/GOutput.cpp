#include "GOutput.h"
#include "GProblem.h"

namespace godzilla {

InputParameters
GOutput::validParams() {
    InputParameters params = Object::validParams();
    params.addPrivateParam<GProblem *>("_gproblem");
    return params;
}

GOutput::GOutput(const InputParameters & params) :
    Object(params),
    GPrintInterface(this),
    problem(*getParam<GProblem *>("_gproblem"))
{
}

} // godzilla
