#include "Godzilla.h"
#include "CallStack.h"
#include "TransientInterface.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
TransientInterface::validParams()
{
    InputParameters params = InputParameters::empty();
    params.add_param<Real>("start_time", 0., "Start time of the simulation");
    params.add_required_param<Real>("end_time", "Simulation end time");
    params.add_required_param<Real>("dt", "Time step size");
    return params;
}

TransientInterface::TransientInterface(const InputParameters & params) :
    ts(nullptr),
    start_time(params.get<Real>("start_time")),
    end_time(params.get<Real>("end_time")),
    dt(params.get<Real>("dt"))
{
    _F_;
}

TransientInterface::~TransientInterface()
{
    _F_;
    TSDestroy(&this->ts);
}

void
TransientInterface::init(const MPI_Comm & comm)
{
    _F_;
    PetscErrorCode ierr;
    ierr = TSCreate(comm, &this->ts);
    checkPetscError(ierr);
}

void
TransientInterface::create(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    setup_time_scheme();

    ierr = TSSetDM(this->ts, dm);
    checkPetscError(ierr);

    ierr = TSSetTime(this->ts, this->start_time);
    checkPetscError(ierr);
    ierr = TSSetMaxTime(this->ts, this->end_time);
    checkPetscError(ierr);
    ierr = TSSetTimeStep(this->ts, this->dt);
    checkPetscError(ierr);
}

void
TransientInterface::setup_time_scheme()
{
    _F_;
    PetscErrorCode ierr;
    // TODO: allow other schemes
    ierr = TSSetType(this->ts, TSBEULER);
    checkPetscError(ierr);
}

void
TransientInterface::solve(Vec x)
{
    _F_;
    TSSolve(this->ts, x);
}

} // namespace godzilla
