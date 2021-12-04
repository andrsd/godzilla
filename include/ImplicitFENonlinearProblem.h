#pragma once

#include "FENonlinearProblem.h"
#include "TransientInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientInterface {
public:
    ImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~ImplicitFENonlinearProblem();

    virtual void create() override;
    virtual void solve() override;
    virtual void run() override;

protected:
    virtual void init() override;
    virtual void setUpCallbacks() override;
    /// Called before the time step solve
    virtual PetscErrorCode onPreStep();
    /// Called after the time step is done solving
    virtual PetscErrorCode onPostStep();
    /// TS monitor callback
    virtual PetscErrorCode tsMonitorCallback(PetscInt stepi, PetscReal time, Vec X);
    /// Setup monitors
    virtual void setUpMonitors() override;
    /// Output
    virtual void output() override;
    /// Output time step
    virtual void outputStep(Vec vec);

public:
    static InputParameters validParams();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode
    __transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx);
};

} // namespace godzilla
