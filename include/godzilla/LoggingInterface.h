// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Logger.h"

namespace godzilla {

/// Logging interface to give objects capability to logger problems like errors and warnings
///
class LoggingInterface {
public:
    explicit LoggingInterface(Logger * /*logger*/) {}
};

} // namespace godzilla
