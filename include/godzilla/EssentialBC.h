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
    const std::vector<Int> & get_components() const;

    /// Evaluate the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    void evaluate(Real time, const Real x[], Scalar u[]);

    /// Evaluate time derivative of the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u  The output field values
    void evaluate_t(Real time, const Real x[], Scalar u[]);

protected:
    template <class T>
    void
    set_compute(T * instance, void (T::*method)(Real, const Real *, Scalar *))
    {
        this->delegate.bind(instance, method);
    }

    template <typename F>
    void
    set_compute(F && func)
    {
        this->delegate.bind(std::forward<F>(func));
    }

    template <class T>
    void
    set_compute_time(T * instance, void (T::*method)(Real, const Real *, Scalar *))
    {
        this->delegate_t.bind(instance, method);
    }

    template <typename F>
    void
    set_compute_time(F && func)
    {
        this->delegate_t.bind(std::forward<F>(func));
    }

private:
    virtual void set_up_callbacks() = 0;
    virtual std::vector<Int> create_components() = 0;

    /// Field ID this boundary condition is attached to
    FieldID fid;
    /// Field name
    Optional<std::string> field_name;
    /// Components
    std::vector<Int> components;
    ///
    FunctionDelegate delegate;
    ///
    FunctionDelegate delegate_t;

public:
    static Parameters parameters();

private:
    static ErrorCode invoke_essential_bc_delegate(Int dim,
                                                  Real time,
                                                  const Real x[],
                                                  Int nc,
                                                  Scalar u[],
                                                  void * ctx);
    static ErrorCode invoke_essential_bc_delegate_t(Int dim,
                                                    Real time,
                                                    const Real x[],
                                                    Int nc,
                                                    Scalar u[],
                                                    void * ctx);
};

} // namespace godzilla
