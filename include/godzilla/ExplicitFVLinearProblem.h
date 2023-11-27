#pragma once

#include "godzilla/NonlinearProblem.h"
#include "godzilla/FVProblemInterface.h"
#include "godzilla/TransientProblemInterface.h"

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
    Real get_time() const override;
    Int get_step_num() const override;

    virtual PetscErrorCode compute_rhs(Real time, const Vector & x, Vector & F);

    const Matrix &
    get_mass_matrix() const
    {
        return this->M;
    }

    const Vector &
    get_lumped_mass_matrix() const
    {
        return this->M_lumped_inv;
    }

protected:
    void init() override;
    void allocate_objects() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void build_local_solution_vector(Vector & loc_sln) override;
    void allocate_mass_matrix();
    void allocate_lumped_mass_matrix();
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
