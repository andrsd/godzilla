// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Types.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    explicit EssentialBC(const Parameters & params);

    void create() override;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    [[nodiscard]] virtual Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    [[nodiscard]] virtual const std::vector<Int> & get_components() const = 0;

    /// Evaluate the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    virtual void evaluate(Real time, const Real x[], Scalar u[]) = 0;

    /// Evaluate time derivative of the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    virtual void evaluate_t(Real time, const Real x[], Scalar u[]) = 0;

    void set_up() override;

private:
    /// Field ID this boundary condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
