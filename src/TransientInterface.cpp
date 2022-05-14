#include "Godzilla.h"
#include "CallStack.h"
#include "TransientInterface.h"
#include "NonlinearProblem.h"
#include "Output.h"
#include "petscdmplex.h"

namespace godzilla {

InputParameters
TransientInterface::valid_params()
{
    InputParameters params;
    params.add_param<PetscReal>("start_time", 0., "Start time of the simulation");
    params.add_required_param<PetscReal>("end_time", "Simulation end time");
    params.add_required_param<PetscReal>("dt", "Time step size");
    return params;
}

TransientInterface::TransientInterface(const InputParameters & params) :
    ts(nullptr),
    start_time(params.get<PetscReal>("start_time")),
    end_time(params.get<PetscReal>("end_time")),
    dt(params.get<PetscReal>("dt")),
    step_num(0)
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
    check_petsc_error(ierr);
}

void
TransientInterface::create(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    set_up_time_scheme();

    ierr = TSSetDM(this->ts, dm);
    check_petsc_error(ierr);

    ierr = TSSetTime(this->ts, this->start_time);
    check_petsc_error(ierr);
    ierr = TSSetMaxTime(this->ts, this->end_time);
    check_petsc_error(ierr);
    ierr = TSSetTimeStep(this->ts, this->dt);
    check_petsc_error(ierr);
    ierr = TSSetStepNumber(this->ts, 1);
    check_petsc_error(ierr);
}

void
TransientInterface::set_up_time_scheme()
{
    _F_;
    PetscErrorCode ierr;
    // TODO: allow other schemes
    ierr = TSSetType(this->ts, TSBEULER);
    check_petsc_error(ierr);
}

void
TransientInterface::solve(Vec x)
{
    _F_;
    TSSolve(this->ts, x);
}

} // namespace godzilla
