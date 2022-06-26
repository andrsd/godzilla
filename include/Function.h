#pragma once

#include "Object.h"
#include "muParser/muParser.h"

namespace godzilla {

/// Base class for function objects
///
class Function : public Object {
public:
    Function(const InputParameters & params);

    /// Register this function with the function parser
    ///
    /// @param parser The mu::Parser object we register this function with
    virtual void register_callback(mu::Parser & parser) = 0;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
