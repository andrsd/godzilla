#include "Godzilla.h"
#include "CallStack.h"
#include "TransientInterface.h"
#include "NonlinearProblem.h"
#include "Output.h"
#include "petscdmplex.h"

namespace godzilla {

PetscErrorCode
__transient_pre_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    TransientInterface * tr = static_cast<TransientInterface *>(ctx);
    return tr->onPreStep();
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    TransientInterface * tr = static_cast<TransientInterface *>(ctx);
    return tr->onPostStep();
}

PetscErrorCode
__transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx)
{
    _F_;
    TransientInterface * tr = static_cast<TransientInterface *>(ctx);
    return tr->tsMonitorCallback(stepi, time, X);
}

InputParameters
TransientInterface::validParams()
{
    InputParameters params = emptyInputParameters();
    params.addParam<PetscReal>("start_time", 0., "Start time of the simulation");
    params.addRequiredParam<PetscReal>("end_time", "Simulation end time");
    params.addRequiredParam<PetscReal>("dt", "Time step size");
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
    ierr = TSSetApplicationContext(this->ts, this);
    checkPetscError(ierr);
}

void
TransientInterface::create(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    setupTimeScheme();

    ierr = TSSetDM(this->ts, dm);
    checkPetscError(ierr);

    setupMonitors();

    ierr = TSSetTime(this->ts, this->start_time);
    checkPetscError(ierr);
    ierr = TSSetMaxTime(this->ts, this->end_time);
    checkPetscError(ierr);
    ierr = TSSetTimeStep(this->ts, this->dt);
    checkPetscError(ierr);
}

void
TransientInterface::setupTimeScheme()
{
    _F_;
    PetscErrorCode ierr;
    // TODO: allow other schemes
    ierr = TSSetType(this->ts, TSBEULER);
    checkPetscError(ierr);
}

PetscErrorCode
TransientInterface::onPreStep()
{
    _F_;
    return 0;
}

PetscErrorCode
TransientInterface::onPostStep()
{
    _F_;
    Vec x;
    TSGetSolution(this->ts, &x);
    VecView(x, PETSC_VIEWER_STDOUT_SELF);
    return 0;
}

PetscErrorCode
TransientInterface::tsMonitorCallback(PetscInt stepi, PetscReal time, Vec X)
{
    _F_;
    return 0;
}

void
TransientInterface::setupMonitors()
{
    _F_;
    PetscErrorCode ierr;
    ierr = TSSetPreStep(this->ts, __transient_pre_step);
    checkPetscError(ierr);
    ierr = TSSetPostStep(this->ts, __transient_post_step);
    checkPetscError(ierr);
    ierr = TSMonitorSet(this->ts, __transient_monitor, this, NULL);
    checkPetscError(ierr);
}

void
TransientInterface::solve(Vec x)
{
    _F_;
    TSSolve(this->ts, x);
}

} // namespace godzilla
