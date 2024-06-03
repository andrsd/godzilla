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
    ~ExplicitProblemInterface();

    /// Get the name of time stepping scheme
    std::string get_scheme() const;

    const Matrix & get_mass_matrix() const;

    Matrix & get_mass_matrix();

    const Vector & get_lumped_mass_matrix() const;

    Vector & get_lumped_mass_matrix();

    ErrorCode compute_rhs(Real time, const Vector & x, Vector & F) override;

protected:
    void set_up_callbacks();
    void set_up_time_scheme() override;
    void allocate_mass_matrix();
    void allocate_lumped_mass_matrix();
    void create_mass_matrix();
    void create_mass_matrix_lumped();
    virtual ErrorCode compute_boundary_local(Real time, Vector & x);
    virtual ErrorCode compute_rhs_local(Real time, const Vector & x, Vector & F) = 0;

private:
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
