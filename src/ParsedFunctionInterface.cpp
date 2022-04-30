#include "ParsedFunctionInterface.h"
#include "App.h"
#include <assert.h>

namespace godzilla {

InputParameters
ParsedFunctionInterface::validParams()
{
    InputParameters params = InputParameters::empty();
    params.add_required_param<std::vector<std::string>>("value", "Function expression.");
    return params;
}

ParsedFunctionInterface::ParsedFunctionInterface(const InputParameters & params) :
    pfi_app(*params.get<const App *>("_app")),
    expression(params.get<std::vector<std::string>>("value"))
{
    this->num_comps = this->expression.size();
    assert(this->num_comps >= 1);
    this->evalr.resize(this->num_comps);
}

void
ParsedFunctionInterface::create()
{
    _F_;
    for (uint i = 0; i < this->num_comps; i++)
        this->evalr[i].create(this->expression[i]);
}

Real
ParsedFunctionInterface::evaluateFunction(uint idx, uint dim, Real time, Real x, Real y, Real z)
{
    return this->evalr[idx].evaluate(dim, time, x, y, z);
}

} // namespace godzilla
