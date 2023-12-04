// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Object.h"

namespace godzilla {

class Problem;
class TransientProblemInterface;

/// Base class for adaptive time stepping
///
class TimeSteppingAdaptor : public Object {
public:
    explicit TimeSteppingAdaptor(const Parameters & params);

    void create() override;

    /// Get TSAdapt object
    ///
    /// @return PETSc TSAdapt object
    [[nodiscard]] TSAdapt get_ts_adapt() const;

    /// Get minimum time step size
    ///
    /// @return Minimum time step size
    [[nodiscard]] Real get_dt_min() const;

    /// Get maximum time step size
    ///
    /// @return Maximum time step size
    [[nodiscard]] Real get_dt_max() const;

protected:
    /// Set time adaptor type
    ///
    /// @param type Type of time adaptor
    void set_type(const char * type);

    /// Set the type of time stepping adaptivity
    virtual void set_type_impl() = 0;

    /// Get problem this time adaptor is part of
    ///
    /// @return Problem this time adaptor is part of
    Problem * get_problem() const;

private:
    /// Problem this adaptor is part of
    Problem * problem;

    /// Transient problem interface this adaptor is part of
    const TransientProblemInterface * tpi;

    /// TSAdapt object
    TSAdapt ts_adapt;

    /// Minimum time step
    const Real & dt_min;

    /// Maximum time step
    const Real & dt_max;

public:
    static Parameters parameters();
};

} // namespace godzilla
