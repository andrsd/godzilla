// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/BoundaryCondition.h"
#include "godzilla/Label.h"
#include "godzilla/Types.h"

namespace godzilla {

namespace internal {

struct NaturalRiemannBCFunctionMethodAbstract {
    virtual ~NaturalRiemannBCFunctionMethodAbstract() = default;
    virtual ErrorCode
    invoke(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) = 0;
};

template <typename T>
struct NaturalRiemannBCFunctionMethod : public NaturalRiemannBCFunctionMethodAbstract {
    NaturalRiemannBCFunctionMethod(
        T * instance,
        void (T::*method)(Real, const Real *, const Real *, const Scalar *, Scalar *)) :
        instance(instance),
        method(method)
    {
    }

    ErrorCode
    invoke(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override
    {
        ((*this->instance).*method)(time, c, n, xI, xG);
        return 0;
    }

private:
    T * instance;
    void (T::*method)(Real, const Real *, const Real *, const Scalar *, Scalar *);
};

} // namespace internal

/// Base class for natural Riemann boundary conditions
class NaturalRiemannBC : public BoundaryCondition {
public:
    explicit NaturalRiemannBC(const Parameters & params);

    void create() override;
    void set_up() override;

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
    /// @param c Centroid
    /// @param n Normal vector
    /// @param xI State at the interior cell
    /// @param xG State at the ghost cell (computed)
    virtual void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) = 0;

private:
    /// Field ID this boundary condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
