#pragma once

#include "FENonlinearProblem.h"
#include "TransientProblemInterface.h"

namespace godzilla {

class ResidualFunc;

class ExplicitFELinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ExplicitFELinearProblem(const Parameters & params);
    ~ExplicitFELinearProblem() override;

    Real get_time() const override;
    Int get_step_num() const override;
    void create() override;
    void check() override;
    bool converged() override;
    void solve() override;

    virtual PetscErrorCode compute_rhs(Real time, const Vector & X, Vector & F);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void set_residual_block(Int field_id,
                            ResidualFunc * f0,
                            ResidualFunc * f1,
                            const Label & label = Label(),
                            Int val = 0) override;
    void create_mass_matrix();
    void create_mass_matrix_lumped();
    virtual PetscErrorCode compute_boundary_local(Real time, Vector & x);
    virtual PetscErrorCode compute_rhs_local(Real time, const Vector & x, Vector & F);

    /// Time stepping scheme
    const std::string & scheme;

    /// Mass matrix
    Matrix M;
    /// Inverse of the lumped mass matrix
    Vector M_lumped_inv;

public:
    static Parameters parameters();
};

} // namespace godzilla
