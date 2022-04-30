#include "ParsedFunctionEvaluator.h"
#include "CallStack.h"

namespace godzilla {

ParsedFunctionEvaluator::ParsedFunctionEvaluator()
{
    _F_;
}

void
ParsedFunctionEvaluator::create(const std::string & expr)
{
    _F_;
    this->parser.SetExpr(expr);
    this->parser.DefineConst("pi", 3.14159265359);
    this->parser.DefineConst("e", 2.71828182846);
}

Real
ParsedFunctionEvaluator::evaluate(uint dim, Real time, Real x, Real y, Real z)
{
    _F_;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &x);
        if (dim >= 2)
            this->parser.DefineVar("y", &y);
        if (dim >= 3)
            this->parser.DefineVar("z", &z);
        return this->parser.Eval();
    }
    catch (mu::Parser::exception_type & e) {
        return std::nan("");
    }
}

} // namespace godzilla
