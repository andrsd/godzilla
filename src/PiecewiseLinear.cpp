// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseLinear.h"
#include "godzilla/LinearInterpolation.h"
#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"

namespace godzilla {

static double
piecewise_linear_function_eval(void * ctx, double x)
{
    auto * fn = static_cast<PiecewiseLinear *>(ctx);
    return fn->evaluate(x);
}

Parameters
PiecewiseLinear::parameters()
{
    Parameters params = Function::parameters();
    params.add_required_param<std::vector<Real>>("x", "Independent variable");
    params.add_required_param<std::vector<Real>>("y", "Dependent variable");
    return params;
}

PiecewiseLinear::PiecewiseLinear(const Parameters & params) : Function(params), linpol(nullptr)
{
    CALL_STACK_MSG();
    try {
        this->linpol = new LinearInterpolation(get_param<std::vector<Real>>("x"),
                                               get_param<std::vector<Real>>("y"));
    }
    catch (std::exception & e) {
        log_error(e.what());
    }
}

PiecewiseLinear::~PiecewiseLinear()
{
    delete this->linpol;
}

void
PiecewiseLinear::register_callback(mu::Parser & parser)
{
    CALL_STACK_MSG();
    parser.DefineFunUserData(get_name(), piecewise_linear_function_eval, this);
}

Real
PiecewiseLinear::evaluate(Real x)
{
    CALL_STACK_MSG();
    return this->linpol->sample(x);
}

} // namespace godzilla
