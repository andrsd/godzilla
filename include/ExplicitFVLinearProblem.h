#pragma once

#include "NonlinearProblem.h"
#include "FVProblemInterface.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ExplicitFVLinearProblem :
    public NonlinearProblem,
    public FVProblemInterface,
    public TransientProblemInterface {
public:
    ExplicitFVLinearProblem(const Parameters & params);
    virtual ~ExplicitFVLinearProblem();

    virtual void create() override;
    virtual void check() override;
    virtual bool converged() override;
    virtual void solve() override;

protected:
    virtual void init() override;
    virtual void allocate_objects() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_initial_guess() override;
    virtual void set_up_time_scheme() override;
    virtual void set_up_monitors() override;

    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
