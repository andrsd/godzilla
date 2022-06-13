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
}

void
FunctionInterface::create()
{
    _F_;
    this->evalr.create(this->expression);
    this->evalr_t.create(this->expression_t);

    assert(this->fi_app != nullptr);
    Problem * p = this->fi_app->get_problem();
    if (p) {
        const auto & funcs = p->get_functions();
        for (auto & f : funcs) {
            this->evalr.register_function(f);
            this->evalr_t.register_function(f);
        }
    }
}

bool
FunctionInterface::evaluate(PetscInt dim,
                            PetscReal time,
                            const PetscReal x[],
                            PetscInt nc,
                            PetscReal u[])
{
    return this->evalr.evaluate(dim, time, x, nc, u);
}

bool
FunctionInterface::evaluate_t(PetscInt dim,
                              PetscReal time,
                              const PetscReal x[],
                              PetscInt nc,
                              PetscReal u[])
{
    return this->evalr_t.evaluate(dim, time, x, nc, u);
}

} // namespace godzilla
