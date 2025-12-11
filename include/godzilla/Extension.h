// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/String.h"

#define EXTENSION_API extern "C"

namespace godzilla {

/// Extension
class Extension {
public:
    /// Create an extension
    ///
    /// @param extension_name Extension name
    Extension(const String & extension_name);

    /// Get extension name
    ///
    /// @return Extension name
    const String & get_name() const;

private:
    /// Extension name
    const String name;
};

} // namespace godzilla
