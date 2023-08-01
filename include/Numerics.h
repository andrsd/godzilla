#pragma once

namespace godzilla {
namespace math {

/// Sign of the argument
///
/// @tparam T Type
/// @param val Value to get the sign of
/// @return Sign of the value, i.e. -1 for negative values, 0 for 0, and 1 for positive values
template <typename T>
int
sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

} // namespace math
} // namespace godzilla
