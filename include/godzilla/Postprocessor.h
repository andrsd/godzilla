// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "godzilla/PrintInterface.h"
#include "petsc.h"

namespace godzilla {

class Problem;

/// Base class for postprocessors
///
class Postprocessor : public Object, public PrintInterface {
public:
    explicit Postprocessor(const Parameters & params);

    /// Compute the postprocessor value
    ///
    virtual void compute() = 0;

    /// Get the computed value
    ///
    /// @return The value computed by the postprocessor
    virtual Real get_value() = 0;

    /// Get problem this post-processor is part of
    ///
    /// @return Problem this postprocessor is part of
    Problem * get_problem() const;

private:
    /// Problem this object is part of
    Problem * problem;

public:
    static Parameters parameters();
};

} // namespace godzilla
