// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Types.h"

namespace godzilla {

class Problem;

struct OutputTag {};

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    using category = OutputTag;

    explicit Output(const Parameters & pars);

    void create() override;

    /// Set execute mask
    ///
    /// @param mask Bit flags for execution
    void set_exec_mask(ExecuteOnFlags flags);

    /// Get execution mask
    ///
    /// @return execution mask
    ExecuteOnFlags execute_on() const;

    /// Should output happen at a specified occasion
    ///
    /// @param mask Bit mask specifying the occasion, see ON_XYZ below
    /// @return `true` if output should happen, otherwise `false`
    virtual bool should_output(ExecuteOn flags);

    /// Output a step of a simulation
    virtual void output_step() = 0;

protected:
    /// Get problem
    ///
    /// @return Problem this output is part of
    Ref<Problem> get_problem() const;

private:
    /// Problem to get data from
    Ref<Problem> problem;

    /// Bitwise mask for determining when this output object should output its content
    ExecuteOnFlags on_mask;

    ///
    Int interval;

    /// Last simulation time when output happened
    Real last_output_time;

public:
    static Parameters parameters();
};

template <typename T>
concept OutputDerived = std::is_base_of_v<Output, T>;

} // namespace godzilla
