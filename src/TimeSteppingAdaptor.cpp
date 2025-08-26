// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/TimeSteppingAdaptor.h"
#include "godzilla/CallStack.h"
#include "godzilla/Problem.h"
#include "godzilla/TransientProblemInterface.h"

namespace godzilla {

Parameters
TimeSteppingAdaptor::parameters()
{
    auto params = Object::parameters();
    params.add_private_param<Problem *>("_problem", nullptr)
        .add_param<Real>("dt_min", PETSC_DEFAULT, "Minimum time step")
        .add_param<Real>("dt_max", PETSC_DEFAULT, "Maximum time step");
    return params;
}

TimeSteppingAdaptor::TimeSteppingAdaptor(const Parameters & params) :
    Object(params),
    problem(get_param<Problem *>("_problem")),
    tpi(dynamic_cast<TransientProblemInterface *>(problem)),
    ts_adapt(nullptr),
    dt_min(get_param<Real>("dt_min")),
    dt_max(get_param<Real>("dt_max"))
{
    CALL_STACK_MSG();
    if (this->tpi)
        PETSC_CHECK(TSGetAdapt(this->tpi->get_ts(), &this->ts_adapt));
}

TSAdapt
TimeSteppingAdaptor::get_ts_adapt() const
{
    CALL_STACK_MSG();
    return this->ts_adapt;
}

Real
TimeSteppingAdaptor::get_dt_min() const
{
    CALL_STACK_MSG();
    return this->dt_min;
}

Real
TimeSteppingAdaptor::get_dt_max() const
{
    CALL_STACK_MSG();
    return this->dt_max;
}

void
TimeSteppingAdaptor::set_type(const char * type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetType(this->ts_adapt, type));
}

Problem *
TimeSteppingAdaptor::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

void
TimeSteppingAdaptor::create()
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetStepLimits(this->ts_adapt, this->dt_min, this->dt_max));
}

void
TimeSteppingAdaptor::set_always_accept(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(TSAdaptSetAlwaysAccept(this->ts_adapt, flag ? PETSC_TRUE : PETSC_FALSE));
}

} // namespace godzilla
