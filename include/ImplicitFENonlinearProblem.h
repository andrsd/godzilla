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
    Real get_time() const override;
    Int get_step_num() const override;

    const Vector & get_solution_vector_local() override;
    virtual PetscErrorCode
    compute_ifunction(Real time, const Vector & X, const Vector & X_t, Vector & F);
    virtual PetscErrorCode compute_ijacobian(Real time,
                                             const Vector & X,
                                             const Vector & X_t,
                                             Real x_t_shift,
                                             Matrix & J,
                                             Matrix & Jp);
    virtual PetscErrorCode compute_boundary(Real time, const Vector & X, const Vector & X_t);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void post_step() override;

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
