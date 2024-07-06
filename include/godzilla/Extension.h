// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

#define EXTENSION_API extern "C"

namespace godzilla {

/// Extension
class Extension {
public:
    /// Create an extension
    ///
    /// @param extension_name Extension name
    Extension(const std::string & extension_name);

    /// Get extension name
    ///
    /// @return Extension name
    [[nodiscard]] const std::string & get_name() const;

private:
    /// Extension name
    const std::string name;
};

} // namespace godzilla
