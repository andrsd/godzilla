#pragma once

#include "Types.h"
#include "NonlinearProblem.h"
#include "FEProblemInterface.h"

namespace godzilla {

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    FENonlinearProblem(const InputParameters & parameters);
    virtual ~FENonlinearProblem();

    virtual void create() override;

protected:
    virtual void init() override;
    virtual void set_up_callbacks() override;
    virtual void set_up_initial_guess() override;
    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

    /// Set up residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    virtual void
    set_residual_block(PetscInt fid, PetscFEResidualFunc * f0, PetscFEResidualFunc * f1);

    /// Set up residual statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    virtual void set_jacobian_block(PetscInt fid,
                                    PetscInt gid,
                                    PetscFEJacobianFunc * g0,
                                    PetscFEJacobianFunc * g1,
                                    PetscFEJacobianFunc * g2,
                                    PetscFEJacobianFunc * g3);

    virtual void on_initial() override;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
