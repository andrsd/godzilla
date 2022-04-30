#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "Space.h"

namespace godzilla {

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    BoundaryCondition(const InputParameters & params);

    /// Get the type of this boundary condition
    ///
    /// @return Type of boundary condition
    virtual BoundaryConditionType get_bc_type() const = 0;

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    virtual const std::string & get_boundary_name() const;

    // virtual void setup(Space & space);

protected:
    /// List of boundary names
    const std::string & boundary;

public:
    /// Method for building InputParameters for this class
    static InputParameters valid_params();
};

} // namespace godzilla
