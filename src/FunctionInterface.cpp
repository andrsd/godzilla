#include "FunctionInterface.h"
#include "App.h"
#include <assert.h>

namespace godzilla {

InputParameters
FunctionInterface::validParams()
{
    InputParameters params = emptyInputParameters();
    params.addRequiredParam<std::vector<std::string>>("value", "Function expression to evaluate.");
    return params;
}

FunctionInterface::FunctionInterface(const InputParameters & params) :
    fi_app(*params.get<const App *>("_app")),
    expression(params.get<std::vector<std::string>>("value"))
{
    this->num_comps = this->expression.size();
    assert(this->num_comps >= 1);
    this->evalr.resize(this->num_comps);
}

void
FunctionInterface::create()
{
    _F_;
    for (unsigned int i = 0; i < this->num_comps; i++)
        this->evalr[i].create(this->expression[i], this->fi_app.functions);
}

PetscReal
FunctionInterface::evaluateFunction(unsigned int idx,
                                    PetscInt dim,
                                    PetscReal time,
                                    const PetscReal x[])
{
    return this->evalr[idx].evaluate(dim, time, x);
}

} // namespace godzilla
