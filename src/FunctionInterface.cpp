#include "FunctionInterface.h"
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
    function_expr(params.get<std::vector<std::string>>("value")),
    num_comps(function_expr.size())
{
    assert(num_comps >= 1);
    this->parser.resize(this->num_comps);
    for (auto & p : this->parser) {
        p.DefineConst("pi", 3.14159265359);
        p.DefineConst("e", 2.71828182846);
    }
}

PetscReal
FunctionInterface::evaluateFunction(unsigned int idx,
                                    PetscInt dim,
                                    PetscReal time,
                                    const PetscReal x[])
{
    PetscReal * xx = const_cast<PetscReal *>(x);
    try {
        auto & fp = this->parser[idx];
        fp.DefineVar("t", &time);
        fp.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            fp.DefineVar("y", &(xx[1]));
        if (dim >= 3)
            fp.DefineVar("z", &(xx[2]));
        fp.SetExpr(this->function_expr[idx]);

        PetscReal v = this->parser[idx].Eval();
        return v;
    }
    catch (mu::Parser::exception_type & e) {
        return std::nan("");
    }
}

} // namespace godzilla
