#include "FunctionInterface.h"
#include "CallStack.h"
#include "App.h"
#include "Problem.h"
#include <assert.h>

namespace godzilla {

InputParameters
FunctionInterface::valid_params()
{
    InputParameters params;
    params.add_required_param<std::vector<std::string>>("value",
                                                        "Function expression to evaluate.");
    return params;
}

InputParameters
FunctionInterface::valid_params_t()
{
    InputParameters params;
    params.add_param<std::vector<std::string>>("value_t",
                                               "Time derivative of the 'value' parameter.");
    return params;
}

FunctionInterface::FunctionInterface(const InputParameters & params) :
    fi_app(params.get<const App *>("_app")),
    expression(params.get<std::vector<std::string>>("value")),
    expression_t(params.has<std::vector<std::string>>("value_t")
                     ? params.get<std::vector<std::string>>("value_t")
                     : std::vector<std::string>())
{
    this->num_comps = this->expression.size();
    assert(this->num_comps >= 1);
    this->evalr.resize(this->num_comps);
    this->evalr_t.resize(this->expression_t.size());
}

void
FunctionInterface::create()
{
    _F_;
    for (unsigned int i = 0; i < this->num_comps; i++)
        this->evalr[i].create(this->expression[i]);
    for (unsigned int i = 0; i < this->evalr_t.size(); i++)
        this->evalr_t[i].create(this->expression_t[i]);

    assert(this->fi_app != nullptr);
    Problem * p = this->fi_app->get_problem();
    if (p) {
        const auto & funcs = p->get_functions();
        for (unsigned int i = 0; i < this->num_comps; i++) {
            for (auto & f : funcs)
                this->evalr[i].register_function(f);
        }
        for (unsigned int i = 0; i < this->evalr_t.size(); i++) {
            for (auto & f : funcs)
                this->evalr_t[i].register_function(f);
        }
    }
}

PetscReal
FunctionInterface::evaluate(unsigned int idx, PetscInt dim, PetscReal time, const PetscReal x[])
{
    return this->evalr[idx].evaluate(dim, time, x);
}

PetscReal
FunctionInterface::evaluate_t(unsigned int idx, PetscInt dim, PetscReal time, const PetscReal x[])
{
    assert(idx < this->evalr_t.size());
    return this->evalr_t[idx].evaluate(dim, time, x);
}

} // namespace godzilla
