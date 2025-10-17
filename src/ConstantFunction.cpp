// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ConstantFunction.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/Validation.h"

namespace godzilla {

namespace {

double
constant_function_eval(void * ctx, double x)
{
    auto * fn = static_cast<ConstantFunction *>(ctx);
    return fn->evaluate(x);
}

} // namespace

Parameters
ConstantFunction::parameters()
{
    auto params = Function::parameters();
    params.add_required_param<Real>("value", "Constant value");
    return params;
}

ConstantFunction::ConstantFunction(const Parameters & pars) :
    Function(pars),
    val(pars.get<Real>("value"))
{
    CALL_STACK_MSG();
}

void
ConstantFunction::register_callback(mu::Parser & parser)
{
    CALL_STACK_MSG();
    parser.DefineFunUserData(get_name(), constant_function_eval, this);
}

void
ConstantFunction::create()
{
    CALL_STACK_MSG();
    Function::create();
}

Real
ConstantFunction::evaluate(Real)
{
    CALL_STACK_MSG();
    return this->val;
}

} // namespace godzilla
