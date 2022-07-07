#pragma once

#include "Object.h"
#include "muParser/muParser.h"

namespace godzilla {

/// Base class for function objects
///
class Function : public Object {
public:
    Function(const Parameters & params);

    /// Register this function with the function parser
    ///
    /// @param parser The mu::Parser object we register this function with
    virtual void register_callback(mu::Parser & parser) = 0;

public:
    static Parameters valid_params();
};

} // namespace godzilla
