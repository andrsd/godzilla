// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Types.h"
#include "godzilla/FunctionDelegate.h"

namespace godzilla {

namespace internal {

struct EssentialBCFunctionMethodAbstract : public FunctionMethodAbstract {
    virtual ErrorCode invoke_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) = 0;
};

template <typename T>
struct EssentialBCFunctionMethod : public EssentialBCFunctionMethodAbstract {
    EssentialBCFunctionMethod(T * instance,
                              void (T::*method)(Real, const Real[], Scalar[]),
                              void (T::*method_t)(Real, const Real[], Scalar[])) :
        instance(instance),
        method(method),
        method_t(method_t)
    {
    }

    ErrorCode
    invoke(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        ((*this->instance).*method)(time, x, u);
        return 0;
    }

    ErrorCode
    invoke_t(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override
    {
        ((*this->instance).*method_t)(time, x, u);
        return 0;
    }

private:
    T * instance;
    void (T::*method)(Real, const Real[], Scalar[]);
    void (T::*method_t)(Real, const Real[], Scalar[]);
};

} // namespace internal

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
