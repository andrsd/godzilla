// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/String.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ImplicitFENonlinearProblem(const Parameters & pars);

    void create() override;
    void solve();
    void run() override;
    bool converged();
    Real get_time() const override;
    Int get_step_num() const override;
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void post_step() override;

    template <class T>
    void
    set_time_boundary_local(T * instance, void (T::*method)(Real, Vector &, Vector &))
    {
        this->compute_boundary_local_method.bind(instance, method);
        PETSC_CHECK(DMTSSetBoundaryLocal(get_dm(),
                                         invoke_compute_boundary_delegate,
                                         &this->compute_boundary_local_method));
    }

private:
    SNESolver create_sne_solver() override;

    /// Form the local residual `f` from the local input `x`
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    /// @param F Local residual vector
    /// @return Error code
    void compute_ifunction_fem(Real time, const Vector & x, const Vector & x_t, Vector & F);

    /// Form the Jacobian `J` from the local input `x`
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    /// @param x_t_shift The multiplicative parameter for dF/du_t
    /// @param J The Jacobian
    /// @param Jp An additional approximation for the Jacobian to be used to compute the
    ///           preconditioner
    void compute_ijacobian_fem(Real time,
                               const Vector & x,
                               const Vector & x_t,
                               Real x_t_shift,
                               Matrix & J,
                               Matrix & Jp);

    /// Insert the essential boundary values into the local vector and the time derivative vector
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    void compute_boundary_fem(Real time, Vector & x, Vector & x_t);

    /// Time stepping scheme
    const String scheme;
    /// Method for essential boundary data for a local implicit function evaluation.
    Delegate<void(Real time, Vector & x, Vector & x_t)> compute_boundary_local_method;

public:
    static Parameters parameters();

private:
    static PetscErrorCode
    invoke_compute_boundary_delegate(DM, Real time, Vec x, Vec x_t, void * context);
};

} // namespace godzilla
