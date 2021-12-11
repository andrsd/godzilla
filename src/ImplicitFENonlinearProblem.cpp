#include "Godzilla.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem.h"
#include "Output.h"
#include "petscts.h"

namespace godzilla {

PetscErrorCode
__transient_pre_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->onPreStep();
}

PetscErrorCode
__transient_post_step(TS ts)
{
    _F_;
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->onPostStep();
}

PetscErrorCode
__transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx)
{
    _F_;
    ImplicitFENonlinearProblem * prob = static_cast<ImplicitFENonlinearProblem *>(ctx);
    return prob->tsMonitorCallback(stepi, time, X);
}

InputParameters
ImplicitFENonlinearProblem::validParams()
{
    InputParameters params = FENonlinearProblem::validParams();
    params += TransientInterface::validParams();
    return params;
}

ImplicitFENonlinearProblem::ImplicitFENonlinearProblem(const InputParameters & params) :
    FENonlinearProblem(params),
    TransientInterface(params)
{
    _F_;
}

ImplicitFENonlinearProblem::~ImplicitFENonlinearProblem()
{
    this->snes = nullptr;
}

void
ImplicitFENonlinearProblem::init()
{
    _F_;
    PetscErrorCode ierr;
    TransientInterface::init(comm());
    ierr = TSSetApplicationContext(this->ts, this);
    checkPetscError(ierr);
    ierr = TSGetSNES(this->ts, &this->snes);
    checkPetscError(ierr);

    FEProblemInterface::init(getDM());
}

void
ImplicitFENonlinearProblem::create()
{
    _F_;
    FENonlinearProblem::create();
    TransientInterface::create(getDM());
}

PetscErrorCode
ImplicitFENonlinearProblem::onPreStep()
{
    _F_;
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::onPostStep()
{
    _F_;
    PetscErrorCode ierr;
    Vec sln;
    ierr = TSGetSolution(this->ts, &sln);
    checkPetscError(ierr);
    computePostprocessors();
    outputStep(sln);
    return 0;
}

PetscErrorCode
ImplicitFENonlinearProblem::tsMonitorCallback(PetscInt stepi, PetscReal t, Vec X)
{
    _F_;
    PetscErrorCode ierr;
    PetscReal dt;
    ierr = TSGetTimeStep(this->ts, &dt);
    checkPetscError(ierr);
    this->time = t;
    godzillaPrint(6, stepi, " Time ", t, ", dt = ", dt);
    return 0;
}

void
ImplicitFENonlinearProblem::solve()
{
    _F_;
    TransientInterface::solve(this->x);
}

void
ImplicitFENonlinearProblem::run()
{
    _F_;
    godzillaPrint(5, "Executing...");
    solve();
}

void
ImplicitFENonlinearProblem::setUpCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = getDM();

    ierr = DMTSSetBoundaryLocal(dm, DMPlexTSComputeBoundary, this);
    checkPetscError(ierr);
    ierr = DMTSSetIFunctionLocal(dm, DMPlexTSComputeIFunctionFEM, this);
    checkPetscError(ierr);
    ierr = DMTSSetIJacobianLocal(dm, DMPlexTSComputeIJacobianFEM, this);
    checkPetscError(ierr);
}

void
ImplicitFENonlinearProblem::setUpMonitors()
{
    _F_;
    FENonlinearProblem::setUpMonitors();

    PetscErrorCode ierr;
    ierr = TSSetPreStep(this->ts, __transient_pre_step);
    checkPetscError(ierr);
    ierr = TSSetPostStep(this->ts, __transient_post_step);
    checkPetscError(ierr);
    ierr = TSMonitorSet(this->ts, __transient_monitor, this, NULL);
    checkPetscError(ierr);
}

void
ImplicitFENonlinearProblem::output()
{
    _F_;
}

void
ImplicitFENonlinearProblem::outputStep(Vec vec)
{
    _F_;
    PetscErrorCode ierr;
    DM dm = getDM();

    PetscInt num;
    ierr = DMGetOutputSequenceNumber(dm, &num, NULL);
    checkPetscError(ierr);

    for (auto & o : this->outputs)
        o->outputStep(num, dm, vec);
}

} // namespace godzilla
