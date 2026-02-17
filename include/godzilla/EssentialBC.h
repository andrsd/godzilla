// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Types.h"
#include "godzilla/Span.h"

namespace godzilla {

/// Essential boundary condition
///
class EssentialBC : public BoundaryCondition {
public:
    explicit EssentialBC(const Parameters & pars);

    void create() override;
    void set_up() override;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    FieldID get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    Span<const Int> get_components() const;

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
    virtual void evaluate_t(Real time, const Real x[], Scalar u[]);

private:
    virtual std::vector<Int> create_components();

    /// Field ID this boundary condition is attached to
    FieldID fid;
    /// Field name
    Optional<String> field_name;
    /// Components
    std::vector<Int> components;

public:
    static Parameters parameters();

private:
    static PetscErrorCode
    invoke_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);
    static PetscErrorCode
    invoke_delegate_t(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);
};

template <typename T>
concept EssentialBCDerived = std::is_base_of_v<EssentialBC, T>;

} // namespace godzilla
