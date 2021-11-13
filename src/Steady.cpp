#include "Godzilla.h"
#include "Steady.h"
#include "CallStack.h"
#include "Problem.h"

namespace godzilla {

registerObject(Steady);

InputParameters
Steady::validParams()
{
    InputParameters params = Executioner::validParams();
    return params;
}

Steady::Steady(const InputParameters & parameters) : Executioner(parameters)
{
    _F_;
}

void
Steady::execute()
{
    _F_;
    godzillaPrint(5, "Executing...");

    this->problem.solve();
    output();
}

} // namespace godzilla
