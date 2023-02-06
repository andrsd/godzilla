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
    explicit ExplicitFVLinearProblem(const Parameters & params);
    ~ExplicitFVLinearProblem() override;

    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

    PetscErrorCode compute_rhs(Real time, Vec x, Vec F);

protected:
    void init() override;
    void allocate_objects() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
