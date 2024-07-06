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

    /// Insert the essential boundary values into the local vector and the time derivative vector
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param x_t Local solution time derivative
    /// @return Error code
    ErrorCode compute_boundary_local(Real time, const Vector & x, const Vector & x_t);
    ErrorCode compute_ifunction(Real time, const Vector & X, const Vector & X_t, Vector & F);
    ErrorCode compute_ijacobian(Real time,
                                const Vector & X,
                                const Vector & X_t,
                                Real x_t_shift,
                                Matrix & J,
                                Matrix & Jp);

private:
    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
