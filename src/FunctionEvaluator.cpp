#include "FunctionEvaluator.h"
#include "CallStack.h"
#include "Function.h"

namespace godzilla {

FunctionEvaluator::FunctionEvaluator()
{
    _F_;
}

void
FunctionEvaluator::create(const std::string & expr)
{
    _F_;
    this->parser.SetExpr(expr);
    this->parser.DefineConst("pi", 3.14159265359);
    this->parser.DefineConst("e", 2.71828182846);
}

PetscReal
FunctionEvaluator::evaluate(PetscInt dim, PetscReal time, const PetscReal x[])
{
    _F_;
    PetscReal * xx = const_cast<PetscReal *>(x);
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            this->parser.DefineVar("y", &(xx[1]));
        if (dim >= 3)
            this->parser.DefineVar("z", &(xx[2]));
        return this->parser.Eval();
    }
    catch (mu::Parser::exception_type & e) {
        return std::nan("");
    }
}

} // namespace godzilla
