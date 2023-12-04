// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <string>

namespace godzilla {
namespace validation {

/// Check that `value` is equal to one of the `options`
///
/// @return `true` if `value` is one of the `options`, `false` otherwise
/// @param value Value to test
/// @param options Possible options
bool in(const std::string & value, const std::vector<std::string> & options);

} // namespace validation
} // namespace godzilla
