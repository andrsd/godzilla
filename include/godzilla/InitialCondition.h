// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Types.h"
#include "godzilla/DenseVector.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for initial conditions
///
class InitialCondition : public Object, public PrintInterface {
public:
    explicit InitialCondition(const Parameters & params);

    void create() override;

    /// Get problem spatial dimension
    ///
    /// @return Spatial dimension
    Int get_dimension() const;

    /// Get field name
    ///
    /// @return The field name
    const std::string & get_field_name() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    Int get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    virtual Int get_num_components() const = 0;

    /// Evaluate the initial condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    virtual void evaluate(Real time, const Real x[], Scalar u[]) = 0;

private:
    /// Discrete problem this object is part of
    DiscreteProblemInterface * dpi;

    /// Field name this initial condition is attached to
    std::string field_name;

    /// Field ID this initial condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
