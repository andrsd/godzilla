// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

namespace internal {

/// Delegate for calling compute_boundary
struct ComputeBoundaryMethodAbstract {
    virtual ~ComputeBoundaryMethodAbstract() = default;
    virtual ErrorCode invoke(Vector & x) = 0;
};

template <typename T>
struct ComputeBoundaryMethod : public ComputeBoundaryMethodAbstract {
    ComputeBoundaryMethod(T * instance, ErrorCode (T::*method)(Vector &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Vector & x) override
    {
        return ((*this->instance).*method)(x);
    }

private:
    T * instance;
    ErrorCode (T::*method)(Vector &);
};

/// Delegate for calling compute_residual
struct ComputeResidualMethodAbstract {
    virtual ~ComputeResidualMethodAbstract() = default;
    virtual ErrorCode invoke(const Vector & x, Vector & f) = 0;
};

template <typename T>
struct ComputeResidualMethod : public ComputeResidualMethodAbstract {
    ComputeResidualMethod(T * instance, ErrorCode (T::*method)(const Vector &, Vector &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(const Vector & x, Vector & f) override
    {
        return ((*this->instance).*method)(x, f);
    }

private:
    T * instance;
    ErrorCode (T::*method)(const Vector &, Vector &);
};

/// Delegate for calling compute_jacobian
struct ComputeJacobianMethodAbstract {
    virtual ~ComputeJacobianMethodAbstract() = default;
    virtual ErrorCode invoke(const Vector & x, Matrix & J, Matrix & Jp) = 0;
};

template <typename T>
struct ComputeJacobianMethod : public ComputeJacobianMethodAbstract {
    ComputeJacobianMethod(T * instance,
                          ErrorCode (T::*method)(const Vector &, Matrix &, Matrix &)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(const Vector & x, Matrix & J, Matrix & Jp) override
    {
        return ((*this->instance).*method)(x, J, Jp);
    }

private:
    T * instance;
    ErrorCode (T::*method)(const Vector &, Matrix &, Matrix &);
};

} // namespace internal

class ResidualFunc;
class JacobianFunc;
class IndexSet;

/// Non-linear problem that arises from a finite element discretization using the PetscFE system
class FENonlinearProblem : public NonlinearProblem, public FEProblemInterface {
public:
    explicit FENonlinearProblem(const Parameters & parameters);
    ~FENonlinearProblem() override;

    void create() override;
    Real get_time() const override;
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
    set_boundary_local(T * instance, ErrorCode (T::*method)(Vector &))
    {
        this->compute_boundary_delegate = new internal::ComputeBoundaryMethod<T>(instance, method);
        PETSC_CHECK(DMSNESSetBoundaryLocal(get_dm(),
                                           FENonlinearProblem::compute_boundary,
                                           this->compute_boundary_delegate));
    }

    /// Set a local residual evaluation function. This function is called with local vector
    /// containing the local vector information PLUS ghost point information.
    template <class T>
    void
    set_function_local(T * instance, ErrorCode (T::*method)(const Vector &, Vector &))
    {
        this->compute_residual_delegate = new internal::ComputeResidualMethod<T>(instance, method);
        PETSC_CHECK(DMSNESSetFunctionLocal(get_dm(),
                                           FENonlinearProblem::compute_residual,
                                           this->compute_residual_delegate));
    }

    /// Set a local Jacobian evaluation function
    template <class T>
    void
    set_jacobian_local(T * instance, ErrorCode (T::*method)(const Vector &, Matrix &, Matrix &))
    {
        this->compute_jacobian_delegate = new internal::ComputeJacobianMethod<T>(instance, method);
        PETSC_CHECK(DMSNESSetJacobianLocal(get_dm(),
                                           FENonlinearProblem::compute_jacobian,
                                           this->compute_jacobian_delegate));
    }

    ErrorCode compute_boundary(Vector & x);

    ErrorCode compute_residual(const Vector & x, Vector & f);
    ErrorCode compute_residual_internal(DM dm,
                                        PetscFormKey key,
                                        const IndexSet & cells,
                                        Real time,
                                        const Vector & loc_x,
                                        const Vector & loc_x_t,
                                        Real t,
                                        Vector & loc_f);
    ErrorCode compute_bnd_residual_internal(DM dm, Vec loc_x, Vec loc_x_t, Real t, Vec loc_f);
    ErrorCode compute_bnd_residual_single_internal(DM dm,
                                                   Real t,
                                                   PetscFormKey key,
                                                   Vec loc_x,
                                                   Vec loc_x_t,
                                                   Vec loc_f,
                                                   DMField coord_field,
                                                   const IndexSet & facets);

    ErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);
    ErrorCode compute_jacobian_internal(DM dm,
                                        PetscFormKey key,
                                        const IndexSet & cell_is,
                                        Real t,
                                        Real x_t_shift,
                                        const Vector & X,
                                        const Vector & X_t,
                                        Matrix & J,
                                        Matrix & Jp);
    ErrorCode compute_bnd_jacobian_internal(DM dm,
                                            Vec X_loc,
                                            Vec X_t_loc,
                                            Real t,
                                            Real x_t_shift,
                                            Mat J,
                                            Mat Jp);
    ErrorCode compute_bnd_jacobian_single_internal(DM dm,
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
    enum State { INITIAL, FINAL } state;
    /// Delegate for compute_boundary
    internal::ComputeBoundaryMethodAbstract * compute_boundary_delegate;
    /// Delegate for compute_residual
    internal::ComputeResidualMethodAbstract * compute_residual_delegate;
    /// Delegate for compute_jacobian
    internal::ComputeJacobianMethodAbstract * compute_jacobian_delegate;

public:
    static Parameters parameters();

private:
    static ErrorCode compute_boundary(DM dm, Vec x, void * context);
    static ErrorCode compute_residual(DM, Vec x, Vec F, void * context);
    static ErrorCode compute_jacobian(DM, Vec x, Mat J, Mat Jp, void * context);
};

} // namespace godzilla
