#pragma once

#include "Types.h"
#include "NonlinearProblem.h"
#include "FEProblemInterface.h"

namespace godzilla {

class ResidualFunc;
class JacobianFunc;
class IndexSet;

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    explicit FENonlinearProblem(const Parameters & parameters);

    void create() override;

    PetscErrorCode compute_residual(Vec x, Vec f) override;
    PetscErrorCode compute_jacobian(Vec x, Mat J, Mat Jp) override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void allocate_objects() override;

    /// Set up residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    /// @param label Region specified by DMLabel where this residual statement is active
    /// @param val Value associated with the region
    virtual void set_residual_block(PetscInt fid,
                                    ResidualFunc * f0,
                                    ResidualFunc * f1,
                                    DMLabel label = nullptr,
                                    PetscInt val = 0);

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

    PetscErrorCode compute_residual_internal(DM dm,
                                             PetscFormKey key,
                                             const IndexSet & cells,
                                             PetscReal time,
                                             Vec loc_x,
                                             Vec loc_x_t,
                                             PetscReal t,
                                             Vec loc_f);
    PetscErrorCode
    compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, PetscReal t, Vec loc_f);
    PetscErrorCode compute_bnd_residual_single_internal(DM dm,
                                                        PetscReal t,
                                                        PetscFormKey key,
                                                        Vec loc_x,
                                                        Vec loc_x_t,
                                                        Vec loc_f,
                                                        DMField coord_field,
                                                        const IndexSet & facets);

public:
    static Parameters parameters();
};

} // namespace godzilla
