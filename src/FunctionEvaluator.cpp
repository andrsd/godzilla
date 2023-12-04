// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FunctionEvaluator.h"
#include "godzilla/CallStack.h"
#include "godzilla/Function.h"
#include "fmt/format.h"
#include <cassert>

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

void
FunctionEvaluator::create(const std::vector<std::string> & expressions)
{
    std::string expr = fmt::to_string(fmt::join(expressions, ","));
    create(expr);
}

void
FunctionEvaluator::define_constant(const std::string & name, Real value)
{
    _F_;
    this->parser.DefineConst(name, value);
}

void
FunctionEvaluator::register_function(Function * fn)
{
    _F_;
    fn->register_callback(this->parser);
}

Real
FunctionEvaluator::evaluate(Int dim, Real time, const Real x[])
{
    _F_;
    auto * xx = const_cast<Real *>(x);
    Real zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim >= 3)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);
        return this->parser.Eval();
    }
    catch (mu::Parser::exception_type & e) {
        error("Function evaluator failed: {}", e.GetMsg());
    }
}

bool
FunctionEvaluator::evaluate(Int dim, Real time, const Real x[], Int nc, Real u[])
{
    _F_;
    auto * xx = const_cast<Real *>(x);
    Real zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim >= 2)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim >= 3)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);

        int n_num;
        mu::value_type * val = this->parser.Eval(n_num);
        assert(nc == n_num);
        for (int i = 0; i < n_num; i++)
            u[i] = val[i];
        return true;
    }
    catch (mu::Parser::exception_type & e) {
        error("Function evaluator failed: {}", e.GetMsg());
    }
}

} // namespace godzilla
