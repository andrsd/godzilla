// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include <string>
#include <tuple>

namespace godzilla {
namespace conv {

/// Convert type to string
///
/// @param value Value to convert
template <typename T>
std::string to_str(T value);

} // namespace conv
} // namespace godzilla
