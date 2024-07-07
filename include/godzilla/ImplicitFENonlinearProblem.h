// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FENonlinearProblem.h"
#include "godzilla/TransientProblemInterface.h"

namespace godzilla {

class ImplicitFENonlinearProblem : public FENonlinearProblem, public TransientProblemInterface {
public:
    explicit ImplicitFENonlinearProblem(const Parameters & params);
    ~ImplicitFENonlinearProblem() override;

    void create() override;
    bool converged() override;
    void solve() override;
    [[nodiscard]] Real get_time() const override;
    [[nodiscard]] Int get_step_num() const override;
    void compute_solution_vector_local() override;

protected:
    void init() override;
    void set_up_callbacks() override;
    void set_up_time_scheme() override;
    void set_up_monitors() override;
    void post_step() override;

    /// Form the local residual `f` from the local input `x`
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    /// @param F Local residual vector
    /// @return Error code
    void compute_ifunction_local(Real time, const Vector & x, const Vector & x_t, Vector & F);

    /// Form the Jacobian `J` from the local input `x`
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    /// @param x_t_shift The multiplicative parameter for dF/du_t
    /// @param J The Jacobian
    /// @param Jp An additional approximation for the Jacobian to be used to compute the
    ///           preconditioner
    void compute_ijacobian_local(Real time,
                                 const Vector & x,
                                 const Vector & x_t,
                                 Real x_t_shift,
                                 Matrix & J,
                                 Matrix & Jp);

private:
    /// Insert the essential boundary values into the local vector and the time derivative vector
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    void compute_boundary_local(Real time, Vector & x, Vector & x_t);

    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
