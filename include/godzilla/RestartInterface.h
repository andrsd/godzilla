// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace godzilla {

class RestartFile;

/// Interface for objects that can be restarted
class RestartInterface {
public:
    RestartInterface() = default;

    /// Write restart file
    ///
    /// @param file File to write restart data to
    virtual void write_restart_file(RestartFile & file) const = 0;

    /// Read restart file
    ///
    /// @param file File to read restart data from
    virtual void read_restart_file(const RestartFile & file) = 0;
};

} // namespace godzilla
