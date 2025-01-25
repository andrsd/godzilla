// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace godzilla {

class RestartInterface {
public:
    RestartInterface();

    virtual void write_restart_file() = 0;
    virtual void read_restart_file() = 0;
};

} // namespace godzilla
