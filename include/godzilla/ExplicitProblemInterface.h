// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"
#include "godzilla/Parameters.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/TransientProblemInterface.h"
#include <string>

namespace godzilla {

class ExplicitProblemInterface : public TransientProblemInterface {
public:
    explicit ExplicitProblemInterface(NonlinearProblem * problem, const Parameters & params);
    ~ExplicitProblemInterface() override;

    /// Get the name of time stepping scheme
    [[nodiscard]] std::string get_scheme() const;

    [[nodiscard]] const Matrix & get_mass_matrix() const;

    Matrix & get_mass_matrix();

    [[nodiscard]] const Vector & get_lumped_mass_matrix() const;

    Vector & get_lumped_mass_matrix();

protected:
    void set_up_callbacks();
    void set_up_time_scheme() override;
    void allocate_mass_matrix();
    void allocate_lumped_mass_matrix();
    void create_mass_matrix();
    void create_mass_matrix_lumped();
    void compute_boundary_local(Real time, Vector & x);

private:
    /// Form the global residual 'F' from the global input 'x' using pointwise functions specified
    /// by the user
    /// @param time The time
    /// @param x Global solution
    /// @param F Global output vector
    void compute_rhs(Real time, const Vector & x, Vector & F);

    /// Form the local residual 'F' from the local input 'x' using pointwise functions specified by
    /// the user
    ///
    /// @param time The time
    /// @param x Local solution
    /// @param F Local output vector
    virtual void compute_rhs_local(Real time, const Vector & x, Vector & F) = 0;

    /// Nonlinear problem
    NonlinearProblem * nl_problem;
    /// Mass matrix
    Matrix M;
    /// Inverse of the lumped mass matrix
    Vector M_lumped_inv;
    /// Time stepping scheme
    const std::string & scheme;

public:
    static Parameters parameters();
};

} // namespace godzilla
