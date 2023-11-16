#include "godzilla/TimeSteppingAdaptor.h"
#include "godzilla/CallStack.h"
#include "godzilla/TransientProblemInterface.h"

namespace godzilla {

Parameters
TimeSteppingAdaptor::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<Problem *>("_problem", nullptr);
    params.add_private_param<const TransientProblemInterface *>("_tpi", nullptr);
    params.add_param<Real>("dt_min", PETSC_DEFAULT, "Minimum time step");
    params.add_param<Real>("dt_max", PETSC_DEFAULT, "Maximum time step");
    return params;
}

TimeSteppingAdaptor::TimeSteppingAdaptor(const Parameters & params) :
    Object(params),
    problem(get_param<Problem *>("_problem")),
    tpi(get_param<const TransientProblemInterface *>("_tpi")),
    ts_adapt(nullptr),
    dt_min(get_param<Real>("dt_min")),
    dt_max(get_param<Real>("dt_max"))
{
    _F_;
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
TimeSteppingAdaptor::create()
{
    _F_;
    PETSC_CHECK(TSGetAdapt(this->tpi->get_ts(), &this->ts_adapt));
    set_type();
    PETSC_CHECK(TSAdaptSetStepLimits(this->ts_adapt, this->dt_min, this->dt_max));
}

} // namespace godzilla
