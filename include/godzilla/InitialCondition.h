// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Types.h"
#include "godzilla/String.h"
#include "godzilla/DenseVector.h"

namespace godzilla {

class DiscreteProblemInterface;

/// Base class for initial conditions
///
class InitialCondition : public Object, public PrintInterface {
public:
    explicit InitialCondition(const Parameters & pars);

    void create() override;

    /// Get problem spatial dimension
    ///
    /// @return Spatial dimension
    Dimension get_dimension() const;

    /// Get field name
    ///
    /// @return The field name
    String get_field_name() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    FieldID get_field_id() const;

    /// Get the number of constrained components
    ///
    /// @return The number of constrained components
    Int get_num_components() const;

    /// Evaluate the initial condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    virtual void evaluate(Real time, const Real x[], Scalar u[]) = 0;

private:
    virtual std::vector<Int> create_components();

    /// Discrete problem this object is part of
    Ref<DiscreteProblemInterface> dpi;
    /// Field name this initial condition is attached to
    Optional<String> field_name;
    /// Field ID this initial condition is attached to
    FieldID fid;
    /// Components
    std::vector<Int> components;

public:
    static Parameters parameters();

private:
    static PetscErrorCode
    invoke_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);

    friend class DiscreteProblemInterface;
};

template <typename T>
concept InitialConditionDerived = std::is_base_of_v<InitialCondition, T>;

} // namespace godzilla
