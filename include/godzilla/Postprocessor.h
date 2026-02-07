// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "godzilla/PrintInterface.h"
#include <type_traits>

namespace godzilla {

class Problem;

struct PostprocessorTag {};

/// Base class for postprocessors
///
class Postprocessor : public Object, public PrintInterface {
public:
    using category = PostprocessorTag;

    explicit Postprocessor(const Parameters & pars);

    /// Get execution mask
    ///
    /// @return execution mask
    ExecuteOnFlags execute_on() const;

    /// Compute the postprocessor value
    ///
    virtual void compute() = 0;

    /// Get the computed value
    ///
    /// @return The value computed by the postprocessor
    virtual std::vector<Real> get_value() = 0;

    /// Should execute happen at a specified occasion
    ///
    /// @param mask Bit mask specifying the occasion, see ON_XYZ below
    /// @return `true` if output should happen, otherwise `false`
    bool should_execute(ExecuteOn flags);

    /// Get problem this post-processor is part of
    ///
    /// @return Problem this postprocessor is part of
    Ref<Problem> get_problem() const;

private:
    /// Problem this object is part of
    Ref<Problem> problem;
    /// Bitwise mask for determining when this output object should output its content
    ExecuteOnFlags on_mask;
    /// Last simulation time when execute happened
    Real last_execute_time;

public:
    static Parameters parameters();
};

template <typename T>
concept PostprocessorDerived = std::is_base_of_v<Postprocessor, T>;

} // namespace godzilla
