#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/TransientProblemInterface.h"

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

    const Matrix & get_mass_matrix() const;

    Matrix & get_mass_matrix();

    const Vector & get_lumped_mass_matrix() const;

    virtual PetscErrorCode compute_rhs(Real time, const Vector & X, Vector & F);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void add_residual_block(Int field_id,
                            ResidualFunc * f0,
                            ResidualFunc * f1,
                            const std::string & region = "") override;
    void build_local_solution_vector(Vector & loc_sln) override;
    void create_mass_matrix();
    void create_mass_matrix_lumped();
    virtual PetscErrorCode compute_boundary_local(Real time, Vector & x);
    virtual PetscErrorCode compute_rhs_local(Real time, const Vector & x, Vector & F);
    void post_step() override;
    const std::string & get_scheme() const;

private:
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
