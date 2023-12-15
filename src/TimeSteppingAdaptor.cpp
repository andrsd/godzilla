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
    Parameters params = Object::parameters();
    params.add_private_param<Problem *>("_problem", nullptr);
    params.add_param<Real>("dt_min", PETSC_DEFAULT, "Minimum time step");
    params.add_param<Real>("dt_max", PETSC_DEFAULT, "Maximum time step");
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
    _F_;
    if (this->tpi)
        PETSC_CHECK(TSGetAdapt(this->tpi->get_ts(), &this->ts_adapt));
}

TSAdapt
TimeSteppingAdaptor::get_ts_adapt() const
{
    _F_;
    return this->ts_adapt;
}

Real
TimeSteppingAdaptor::get_dt_min() const
{
    _F_;
    return this->dt_min;
}

Real
TimeSteppingAdaptor::get_dt_max() const
{
    _F_;
    return this->dt_max;
}

void
TimeSteppingAdaptor::set_type(const char * type)
{
    _F_;
    PETSC_CHECK(TSAdaptSetType(this->ts_adapt, type));
}

Problem *
TimeSteppingAdaptor::get_problem() const
{
    _F_;
    return this->problem;
}

void
TimeSteppingAdaptor::create()
{
    _F_;
    PETSC_CHECK(TSAdaptSetStepLimits(this->ts_adapt, this->dt_min, this->dt_max));
}

void
TimeSteppingAdaptor::set_always_accept(bool flag)
{
    _F_;
    PETSC_CHECK(TSAdaptSetAlwaysAccept(this->ts_adapt, flag ? PETSC_TRUE : PETSC_FALSE));
}

} // namespace godzilla
