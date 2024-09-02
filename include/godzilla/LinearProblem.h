// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Problem.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Preconditioner.h"

namespace godzilla {

/// Linear problem
///
class LinearProblem : public Problem {
public:
    explicit LinearProblem(const Parameters & parameters);
    ~LinearProblem() override;
    void create() override;
    void run() override;

    /// true if solve converged, otherwise false
    bool converged();

protected:
    /// Initialize the problem
    virtual void init();

    /// Allocate Jacobian/residual objects
    void allocate_objects() override;

    /// Setup computation of residual and Jacobian callbacks
    virtual void set_up_callbacks();

    /// Setup monitors
    virtual void set_up_monitors();

    /// Setup solver parameters
    virtual void set_up_solver_parameters();

    /// Method for setting matrix properties
    virtual void set_up_matrix_properties();

    /// Method for creating a preconditioner
    virtual Preconditioner create_preconditioner(PC pc);

    /// Method to compute right-hand side
    virtual void compute_rhs(Vector & b) = 0;

    /// Method to compute operators
    virtual void compute_operators(Matrix & A, Matrix & B) = 0;

    /// Monitor callback
    virtual void monitor(Int it, Real rnorm);

    void solve();

private:
    /// KSP object
    KrylovSolver ks;
    /// The right-hand side vector
    Vector b;
    /// Preconditioner
    Preconditioner pc;
    /// Relative convergence tolerance for the linear solver
    Real lin_rel_tol;
    /// Absolute convergence tolerance for the linear solver
    Real lin_abs_tol;
    /// Maximum number of iterations for the linear solver
    Int lin_max_iter;

public:
    static Parameters parameters();
};

} // namespace godzilla
