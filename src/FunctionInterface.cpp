#include "FunctionInterface.h"
#include "App.h"
#include "Problem.h"
#include <assert.h>

namespace godzilla {

InputParameters
FunctionInterface::valid_params()
{
    InputParameters params;
    params.add_required_param<std::vector<std::string>>("value", "Function expression to evaluate.");
    return params;
}

FunctionInterface::FunctionInterface(const InputParameters & params) :
    fi_app(params.get<const App *>("_app")),
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
        this->evalr[i].create(this->expression[i]);

    assert(this->fi_app != nullptr);
    Problem * p = this->fi_app->get_problem();
    if (p) {
        const auto & funcs = p->get_functions();
        for (unsigned int i = 0; i < this->num_comps; i++) {
            for (auto & f : funcs)
                this->evalr[i].register_function(f);
        }
    }
}

PetscReal
FunctionInterface::evaluate_function(unsigned int idx,
                                    PetscInt dim,
                                    PetscReal time,
                                    const PetscReal x[])
{
    return this->evalr[idx].evaluate(dim, time, x);
}

} // namespace godzilla
