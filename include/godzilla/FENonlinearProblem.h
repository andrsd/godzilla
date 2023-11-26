#pragma once

#include "godzilla/Types.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

class ResidualFunc;
class JacobianFunc;
class IndexSet;

/// PETSc non-linear problem that arises from a finite element discretization using the PetscFE
/// system
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    explicit FENonlinearProblem(const Parameters & parameters);

    void create() override;
    PetscErrorCode compute_residual(const Vector & x, Vector & f) override;
    PetscErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp) override;

    /// Method for computing boundary values
    virtual PetscErrorCode compute_boundary(Vector & x);

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void allocate_objects() override;
    void on_initial() override;
    void build_local_solution_vector(Vector & loc_sln) override;

    virtual PetscErrorCode compute_residual_internal(DM dm,
                                                     PetscFormKey key,
                                                     const IndexSet & cells,
                                                     Real time,
                                                     const Vector & loc_x,
                                                     const Vector & loc_x_t,
                                                     Real t,
                                                     Vector & loc_f);
    PetscErrorCode compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, Real t, Vec loc_f);
    PetscErrorCode compute_bnd_residual_single_internal(DM dm,
                                                        Real t,
                                                        PetscFormKey key,
                                                        Vec loc_x,
                                                        Vec loc_x_t,
                                                        Vec loc_f,
                                                        DMField coord_field,
                                                        const IndexSet & facets);

    virtual PetscErrorCode compute_jacobian_internal(DM dm,
                                                     PetscFormKey key,
                                                     const IndexSet & cell_is,
                                                     Real t,
                                                     Real x_t_shift,
                                                     const Vector & X,
                                                     const Vector & X_t,
                                                     Matrix & J,
                                                     Matrix & Jp);
    PetscErrorCode compute_bnd_jacobian_internal(DM dm,
                                                 Vec X_loc,
                                                 Vec X_t_loc,
                                                 Real t,
                                                 Real x_t_shift,
                                                 Mat J,
                                                 Mat Jp);
    PetscErrorCode compute_bnd_jacobian_single_internal(DM dm,
                                                        Real t,
                                                        const Label & label,
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
