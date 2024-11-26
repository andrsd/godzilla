// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Types.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    explicit Output(const Parameters & params);

    void create() override;

    /// Set execute mask
    ///
    /// @param mask Bit mask for execution
    void set_exec_mask(ExecuteOn mask);

    /// Get execution mask
    ///
    /// @return execution mask
    ExecuteOn get_exec_mask() const;

    /// Should output happen at a specified occasion
    ///
    /// @param mask Bit mask specifying the occasion, see ON_XYZ below
    /// @return `true` if output should happen, otherwise `false`
    virtual bool should_output(ExecuteOnFlag mask);

    /// Output a step of a simulation
    virtual void output_step() = 0;

protected:
    /// Get problem
    ///
    /// @return Problem this output is part of
    Problem * get_problem() const;

    /// Set up the execution mask
    void set_up_exec();

private:
    /// Problem to get data from
    Problem * problem;

    /// Bitwise mask for determining when this output object should output its content
    ExecuteOn on_mask;

    ///
    Int interval;

    /// Last simulation time when output happened
    Real last_output_time;

public:
    static Parameters parameters();
};

} // namespace godzilla
