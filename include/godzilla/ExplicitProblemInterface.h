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

    const Matrix & get_mass_matrix() const;

    Matrix & get_mass_matrix();

    const Vector & get_lumped_mass_matrix() const;

    Vector & get_lumped_mass_matrix();

    virtual PetscErrorCode compute_rhs(Real time, const Vector & x, Vector & F);

protected:
    void check();
    void set_up_callbacks();
    void set_up_time_scheme();
    void allocate_mass_matrix();
    void allocate_lumped_mass_matrix();
    void create_mass_matrix();
    void create_mass_matrix_lumped();
    virtual PetscErrorCode compute_boundary_local(Real time, Vector & x);
    virtual PetscErrorCode compute_rhs_local(Real time, const Vector & x, Vector & F) = 0;

private:
    /// Nonlinear problem
    NonlinearProblem * nl_problem;
    /// Mass matrix
    Matrix M;
    /// Inverse of the lumped mass matrix
    Vector M_lumped_inv;

public:
    static Parameters parameters();
};

} // namespace godzilla
