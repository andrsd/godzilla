// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Types.h"
#include "godzilla/Label.h"

namespace godzilla {

class FEProblemInterface;
class BndResidualFunc;
class BndJacobianFunc;

/// Base class for natural boundary conditions
class NaturalBC : public BoundaryCondition {
public:
    explicit NaturalBC(const Parameters & params);

    void create() override;
    void set_up() override;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    [[nodiscard]] Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    [[nodiscard]] virtual const std::vector<Int> & get_components() const = 0;

    /// Set up the weak form for the boundary integral of this boundary condition
    virtual void set_up_weak_form() = 0;

protected:
    /// Set residual statement for the boundary integral
    ///
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void add_residual_block(BndResidualFunc * f0, BndResidualFunc * f1);

    /// Set Jacobian statement for the boundary integral
    ///
    /// @param gid Field ID
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void add_jacobian_block(Int gid,
                            BndJacobianFunc * g0,
                            BndJacobianFunc * g1,
                            BndJacobianFunc * g2,
                            BndJacobianFunc * g3);

private:
    /// Field ID this boundary condition is attached to
    Int fid;

    /// Finite element problem this object is part of
    FEProblemInterface * fepi;

public:
    static Parameters parameters();
};

} // namespace godzilla
