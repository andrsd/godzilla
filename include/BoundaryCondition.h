#pragma once

#include "GodzillaConfig.h"
#include "Types.h"
#include "Object.h"
#include "PrintInterface.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    explicit BoundaryCondition(const Parameters & params);
    ~BoundaryCondition() override = default;

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    NO_DISCARD virtual const std::string & get_boundary() const;

    /// Get DiscreteProblemInterface
    ///
    /// @return Discrete problem this BC is part of
    NO_DISCARD virtual const DiscreteProblemInterface * get_discrete_problem_interface() const;

    /// Set up this boundary condition
    virtual void set_up() = 0;

protected:
    /// Discrete problem this object is part of
    const DiscreteProblemInterface * dpi;

    /// List of boundary names
    const std::string & boundary;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
