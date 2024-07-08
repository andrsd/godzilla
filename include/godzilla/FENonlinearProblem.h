// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Delegate.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

class ResidualFunc;
class JacobianFunc;
class IndexSet;

/// Non-linear problem that arises from a finite element discretization using the PetscFE system
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    explicit FENonlinearProblem(const Parameters & parameters);

    void create() override;
    [[nodiscard]] Real get_time() const override;
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_initial_guess() override;
    void allocate_objects() override;
    void on_initial() override;
    void on_final() override;

    /// Set a function to insert, for example, essential boundary conditions into a ghosted solution
    /// vector
    template <class T>
    void
    set_boundary_local(T * instance, void (T::*method)(Vector &))
    {
        this->compute_boundary_delegate.bind(instance, method);
        PETSC_CHECK(DMSNESSetBoundaryLocal(get_dm(),
                                           invoke_compute_boundary_delegate,
                                           &this->compute_boundary_delegate));
    }

    /// Set a local residual evaluation function. This function is called with local vector
    /// containing the local vector information PLUS ghost point information.
    template <class T>
    void
    set_function_local(T * instance, void (T::*method)(const Vector &, Vector &))
    {
        this->compute_residual_delegate.bind(instance, method);
        PETSC_CHECK(DMSNESSetFunctionLocal(get_dm(),
                                           invoke_compute_residual_delegate,
                                           &this->compute_residual_delegate));
    }

    /// Set a local Jacobian evaluation function
    template <class T>
    void
    set_jacobian_local(T * instance, void (T::*method)(const Vector &, Matrix &, Matrix &))
    {
        this->compute_jacobian_delegate.bind(instance, method);
        PETSC_CHECK(DMSNESSetJacobianLocal(get_dm(),
                                           invoke_compute_jacobian_delegate,
                                           &this->compute_jacobian_delegate));
    }

    void compute_boundary(Vector & x);

    void compute_residual_internal(DM dm,
                                   PetscFormKey key,
                                   const IndexSet & cells,
                                   Real time,
                                   const Vector & loc_x,
                                   const Vector & loc_x_t,
                                   Real t,
                                   Vector & loc_f);
    void compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, Real t, Vec loc_f);
    void compute_bnd_residual_single_internal(DM dm,
                                              Real t,
                                              PetscFormKey key,
                                              Vec loc_x,
                                              Vec loc_x_t,
                                              Vec loc_f,
                                              DMField coord_field,
                                              const IndexSet & facets);

    void compute_jacobian_internal(DM dm,
                                   PetscFormKey key,
                                   const IndexSet & cell_is,
                                   Real t,
                                   Real x_t_shift,
                                   const Vector & X,
                                   const Vector & X_t,
                                   Matrix & J,
                                   Matrix & Jp);
    void compute_bnd_jacobian_internal(DM dm,
                                       Vec X_loc,
                                       Vec X_t_loc,
                                       Real t,
                                       Real x_t_shift,
                                       Mat J,
                                       Mat Jp);
    void compute_bnd_jacobian_single_internal(DM dm,
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

private:
    void compute_residual(const Vector & x, Vector & f);
    void compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);

    enum State { INITIAL, FINAL } state;
    /// Delegate for compute_boundary
    Delegate<void(Vector &)> compute_boundary_delegate;
    /// Delegate for compute_residual
    Delegate<void(const Vector &, Vector &)> compute_residual_delegate;
    /// Delegate for compute_jacobian
    Delegate<void(const Vector & x, Matrix & J, Matrix & Jp)> compute_jacobian_delegate;

public:
    static Parameters parameters();

private:
    static ErrorCode invoke_compute_boundary_delegate(DM dm, Vec x, void * context);
    static ErrorCode invoke_compute_residual_delegate(DM, Vec x, Vec F, void * context);
    static ErrorCode invoke_compute_jacobian_delegate(DM, Vec x, Mat J, Mat Jp, void * context);
};

} // namespace godzilla
