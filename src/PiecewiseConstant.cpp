// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PiecewiseConstant.h"
#include "godzilla/Godzilla.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include "godzilla/Validation.h"

namespace godzilla {

REGISTER_OBJECT(PiecewiseConstant);

static double
piecewise_constant_function_eval(void * ctx, double x)
{
    auto * fn = static_cast<PiecewiseConstant *>(ctx);
    return fn->evaluate(x);
}

Parameters
PiecewiseConstant::parameters()
{
    Parameters params = Function::parameters();
    params.add_required_param<std::vector<Real>>("x", "Independent variable");
    params.add_required_param<std::vector<Real>>("y", "Dependent variable");
    params.add_param<std::string>("continuity",
                                  "right",
                                  "Continuity of the function: [left, right]");
    return params;
}

PiecewiseConstant::PiecewiseConstant(const Parameters & params) :
    Function(params),
    continuity(RIGHT),
    x(get_param<std::vector<Real>>("x")),
    y(get_param<std::vector<Real>>("y"))
{
    CALL_STACK_MSG();
    if (this->x.size() + 1 != this->y.size())
        log_error("Size of 'x' ({}) does not match size of 'y' ({}).",
                  this->x.size(),
                  this->y.size());

    if (this->x.size() == 0)
        log_error("Size of 'x' is {}. It must be 1 or more.", this->x.size());
    else {
        // check monotonicity
        for (std::size_t i = 0; i < this->x.size() - 1; i++) {
            if (this->x[i] >= this->x[i + 1])
                log_error("Values in 'x' must be increasing. Failed at index '{}'.", i + 1);
        }
    }
}

void
PiecewiseConstant::register_callback(mu::Parser & parser)
{
    CALL_STACK_MSG();
    parser.DefineFunUserData(get_name(), piecewise_constant_function_eval, this);
}

void
PiecewiseConstant::create()
{
    CALL_STACK_MSG();
    auto cont = get_param<std::string>("continuity");
    if (validation::in(cont, { "left", "right" })) {
        std::string cont_lc = utils::to_lower(cont);
        if (cont_lc == "left")
            this->continuity = LEFT;
        else if (cont_lc == "right")
            this->continuity = RIGHT;
    }
    else
        log_error("The 'continuity' parameter can be either 'left' or 'right'.");
}

Real
PiecewiseConstant::evaluate(Real x)
{
    CALL_STACK_MSG();
    switch (this->continuity) {
    case RIGHT:
        return eval_right_cont(x);
    case LEFT:
        return eval_left_cont(x);
    }
}

Real
PiecewiseConstant::eval_right_cont(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x.size();
    if (x < this->x[0])
        return this->y[0];
    else if (x >= this->x[sz - 1])
        return this->y[sz];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y[lo_idx + 1];
            }
            else {
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x < this->x[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

Real
PiecewiseConstant::eval_left_cont(Real x)
{
    CALL_STACK_MSG();
    std::size_t sz = this->x.size();
    if (x <= this->x[0])
        return this->y[0];
    else if (x > this->x[sz - 1])
        return this->y[sz];
    else {
        std::size_t lo_idx = 0;
        std::size_t hi_idx = sz - 1;
        while (true) {
            if (lo_idx + 1 == hi_idx) {
                return this->y[lo_idx + 1];
            }
            else {
                std::size_t mid_idx = (lo_idx + hi_idx) / 2;
                if (x <= this->x[mid_idx])
                    hi_idx = mid_idx;
                else
                    lo_idx = mid_idx;
            }
        }
    }
}

} // namespace godzilla
