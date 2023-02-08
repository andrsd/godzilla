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

    PetscErrorCode compute_residual(const Vector & x, Vector & f) override;
    PetscErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp) override;

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
    virtual void set_residual_block(Int fid,
                                    ResidualFunc * f0,
                                    ResidualFunc * f1,
                                    DMLabel label = nullptr,
                                    Int val = 0);

    /// Set up Jacobian statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    /// @param label Region specified by DMLabel where this residual statement is active
    /// @param val Value associated with the region
    virtual void set_jacobian_block(Int fid,
                                    Int gid,
                                    JacobianFunc * g0,
                                    JacobianFunc * g1,
                                    JacobianFunc * g2,
                                    JacobianFunc * g3,
                                    DMLabel label = nullptr,
                                    Int val = 0);

    void on_initial() override;

    PetscErrorCode compute_residual_internal(DM dm,
                                             PetscFormKey key,
                                             const IndexSet & cells,
                                             Real time,
                                             Vec loc_x,
                                             Vec loc_x_t,
                                             Real t,
                                             Vec loc_f);
    PetscErrorCode compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, Real t, Vec loc_f);
    PetscErrorCode compute_bnd_residual_single_internal(DM dm,
                                                        Real t,
                                                        PetscFormKey key,
                                                        Vec loc_x,
                                                        Vec loc_x_t,
                                                        Vec loc_f,
                                                        DMField coord_field,
                                                        const IndexSet & facets);

    PetscErrorCode compute_jacobian_internal(DM dm,
                                             PetscFormKey key,
                                             const IndexSet & cell_is,
                                             Real t,
                                             Real x_t_shift,
                                             Vec X,
                                             Vec X_t,
                                             Mat J,
                                             Mat Jp);
    PetscErrorCode compute_bnd_jacobian_internal(DM dm,
                                                 Vec X_loc,
                                                 Vec X_t_loc,
                                                 Real t,
                                                 Real x_t_shift,
                                                 Mat J,
                                                 Mat Jp);
    PetscErrorCode compute_bnd_jacobian_single_internal(DM dm,
                                                        Real t,
                                                        DMLabel label,
                                                        Int n_values,
                                                        const Int values[],
                                                        Int field_i,
                                                        Vec X_loc,
                                                        Vec X_t_loc,
                                                        Real x_t_shift,
                                                        Mat J,
                                                        Mat Jp,
                                                        DMField coord_field,
                                                        const IndexSet & facets);

public:
    static Parameters parameters();
};

} // namespace godzilla
