// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FunctionEvaluator.h"
#include "godzilla/CallStack.h"
#include "godzilla/Function.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace godzilla {

FunctionEvaluator::FunctionEvaluator()
{
    CALL_STACK_MSG();
}

void
FunctionEvaluator::create(const std::string & expr)
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
    this->parser.DefineConst(name, value);
}

void
FunctionEvaluator::register_function(Function * fn)
{
    CALL_STACK_MSG();
    fn->register_callback(this->parser);
}

Real
FunctionEvaluator::evaluate(Dimension dim, Real time, const Real x[])
{
    CALL_STACK_MSG();
    auto * xx = const_cast<Real *>(x);
    Real zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim == 2_D || dim == 3_D)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim == 3_D)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);
        return this->parser.Eval();
    }
    catch (mu::Parser::exception_type & e) {
        throw Exception("Function evaluator failed: {}", e.GetMsg());
    }
}

bool
FunctionEvaluator::evaluate(Dimension dim, Real time, const Real x[], Int nc, Real u[])
{
    CALL_STACK_MSG();
    auto * xx = const_cast<Real *>(x);
    Real zero = 0.;
    try {
        this->parser.DefineVar("t", &time);
        this->parser.DefineVar("x", &(xx[0]));
        if (dim == 2_D || dim == 3_D)
            this->parser.DefineVar("y", &(xx[1]));
        else
            this->parser.DefineVar("y", &zero);
        if (dim == 3_D)
            this->parser.DefineVar("z", &(xx[2]));
        else
            this->parser.DefineVar("z", &zero);

        int n_num;
        mu::value_type * val = this->parser.Eval(n_num);
        assert_true(nc == n_num,
                    "Number of computed values does not match the number of components specified");
        for (int i = 0; i < n_num; ++i)
            u[i] = val[i];
        return true;
    }
    catch (mu::Parser::exception_type & e) {
        throw Exception("Function evaluator failed: {}", e.GetMsg());
    }
}

} // namespace godzilla
