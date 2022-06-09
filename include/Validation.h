#pragma once

#include <vector>
#include <string>

namespace godzilla {
namespace validation {

/// Check that `param` attains one of the `options`
///
/// @return `true` if name is one f the options, `false` otherwise
/// @param value Value to test
/// @param options Possible options that value can have
bool in(const std::string & value, const std::vector<std::string> & options);

} // namespace validation
} // namespace godzilla
