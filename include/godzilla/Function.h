// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Types.h"
#include "muParser/muParser.h"

namespace godzilla {

class Problem;

/// Base class for function objects
///
class Function : public Object {
public:
    explicit Function(const Parameters & pars);

    /// Register this function with the function parser
    ///
    /// @param parser The mu::Parser object we register this function with
    virtual void register_callback(mu::Parser & parser) = 0;

    /// Get problem spatial dimension
    ///
    /// @return Spatial dimension
    Dimension get_dimension() const;

public:
    static Parameters parameters();
};

} // namespace godzilla
