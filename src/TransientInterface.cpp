#include "Godzilla.h"
#include "CallStack.h"
#include "TransientInterface.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
TransientInterface::validParams()
{
    InputParameters params = empty_input_parameters();
    params.add_param<PetscReal>("start_time", 0., "Start time of the simulation");
    params.add_required_param<PetscReal>("end_time", "Simulation end time");
    params.add_required_param<PetscReal>("dt", "Time step size");
    return params;
}

TransientInterface::TransientInterface(const InputParameters & params) :
    ts(nullptr),
    start_time(params.get<PetscReal>("start_time")),
    end_time(params.get<PetscReal>("end_time")),
    dt(params.get<PetscReal>("dt"))
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
