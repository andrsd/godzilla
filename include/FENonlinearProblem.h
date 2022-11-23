#pragma once

#include "Types.h"
#include "NonlinearProblem.h"
#include "FEProblemInterface.h"

namespace godzilla {

class ResidualFunc;
class JacobianFunc;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    FENonlinearProblem(const Parameters & parameters);
    virtual ~FENonlinearProblem();

    void create() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void allocate_objects() override;
    PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

    /// Set up residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    virtual void set_residual_block(PetscInt fid, ResidualFunc * f0, ResidualFunc * f1);

    /// Set up Jacobian statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    virtual void set_jacobian_block(PetscInt fid,
                                    PetscInt gid,
                                    JacobianFunc * g0,
                                    JacobianFunc * g1,
                                    JacobianFunc * g2,
                                    JacobianFunc * g3);

    void on_initial() override;

public:
    static Parameters parameters();

    friend PetscErrorCode __fep_compute_residual(DM dm, Vec x, Vec F, void * user);
    friend PetscErrorCode __fep_compute_jacobian(DM dm, Vec x, Mat J, Mat Jp, void * user);
};

} // namespace godzilla
