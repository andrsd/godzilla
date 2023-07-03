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

    virtual PetscErrorCode compute_rhs(Real time, const Vector & x, Vector & F);

protected:
    void init() override;
    void allocate_objects() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
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
