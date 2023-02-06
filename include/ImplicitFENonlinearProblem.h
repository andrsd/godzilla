#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ImplicitFENonlinearProblem(const Parameters & params);
    ~ImplicitFENonlinearProblem() override;

    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

    virtual PetscErrorCode compute_ifunction(Real time, Vec X, Vec X_t, Vec F);
    virtual PetscErrorCode
    compute_ijacobian(Real time, Vec X, Vec X_t, Real x_t_shift, Mat J, Mat Jp);
    virtual PetscErrorCode compute_boundary(Real time, Vec X, Vec X_t);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
