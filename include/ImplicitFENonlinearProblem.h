#pragma once

#include "FENonlinearProblem.h"
#include "TransientInterface.h"

namespace godzilla {

/// Transient implicit finite element problem
///
class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientInterface {
public:
    ImplicitFENonlinearProblem(const InputParameters & params);
    virtual ~ImplicitFENonlinearProblem();

    virtual void create() override;
    virtual void solve() override;
    virtual void run() override;

protected:
    virtual void init() override;
    virtual void setup_callbacks() override;
    /// Called before the time step solve
    virtual PetscErrorCode on_pre_step();
    /// Called after the time step is done solving
    virtual PetscErrorCode on_post_step();
    /// TS monitor callback
    virtual PetscErrorCode ts_monitor_callback(PetscInt stepi, PetscReal time, Vec X);
    /// Setup monitors
    virtual void setup_monitors() override;

public:
    static InputParameters validParams();

    friend PetscErrorCode __transient_pre_step(TS ts);
    friend PetscErrorCode __transient_post_step(TS ts);
    friend PetscErrorCode
    __transient_monitor(TS ts, PetscInt stepi, PetscReal time, Vec X, void * ctx);
};

} // namespace godzilla
