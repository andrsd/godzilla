// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/String.h"

namespace godzilla {
namespace conv {

/// Convert type to string
///
/// @param value Value to convert
template <typename T>
String to_str(T value);

} // namespace conv
} // namespace godzilla
