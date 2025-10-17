// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Object.h"
#include "petscts.h"

namespace godzilla {

class Problem;
class TransientProblemInterface;

/// Base class for adaptive time stepping
///
class TimeSteppingAdaptor : public Object {
public:
    explicit TimeSteppingAdaptor(const Parameters & pars);

    void create() override;

    /// Get TSAdapt object
    ///
    /// @return PETSc TSAdapt object
    TSAdapt get_ts_adapt() const;

    /// Get minimum time step size
    ///
    /// @return Minimum time step size
    Real get_dt_min() const;

    /// Get maximum time step size
    ///
    /// @return Maximum time step size
    Real get_dt_max() const;

    /// Set time adaptor type
    ///
    /// @param type Type of time adaptor
    void set_type(const char * type);

    /// Set whether to always accept steps regardless of any error or stability condition not
    /// meeting the prescribed goal
    ///
    /// @param flag Whether to always accept steps
    void set_always_accept(bool flag);

protected:
    /// Get problem this time adaptor is part of
    ///
    /// @return Problem this time adaptor is part of
    Problem * get_problem() const;

private:
    /// Problem this adaptor is part of
    Problem * problem;

    /// Transient problem interface this adaptor is part of
    TransientProblemInterface * tpi;

    /// TSAdapt object
    TSAdapt ts_adapt;

    /// Minimum time step
    Real dt_min;

    /// Maximum time step
    Real dt_max;

public:
    static Parameters parameters();
};

} // namespace godzilla
