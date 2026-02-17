// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Object.h"
#include "godzilla/String.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Span.h"

namespace godzilla {

class UnstructuredMesh;
class Problem;
class DiscreteProblemInterface;

/// Base class for boundary conditions
///
class BoundaryCondition : public Object, public PrintInterface {
public:
    explicit BoundaryCondition(const Parameters & pars);

    /// Get problem spatial dimension
    ///
    /// @return Spatial dimension
    Dimension get_dimension() const;

    /// Get the boundary name this BC is active on
    ///
    /// @return The boundary name
    Span<const String> get_boundary() const;

    /// Get DiscreteProblemInterface
    ///
    /// @return Discrete problem this BC is part of
    Ref<DiscreteProblemInterface> get_discrete_problem_interface() const;

    /// Set up this boundary condition
    virtual void set_up() = 0;

protected:
    /// Get problem this auxiliary field is part of
    Ref<Problem> get_problem() const;

private:
    /// Discrete problem this object is part of
    Ref<DiscreteProblemInterface> dpi;

    /// List of boundary names
    const std::vector<String> boundary;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

template <typename T>
concept BoundaryConditionDerived = std::is_base_of_v<BoundaryCondition, T>;

} // namespace godzilla
