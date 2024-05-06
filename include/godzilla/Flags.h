// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <initializer_list>

namespace godzilla {

/// Template for creating a type that can do a bit-wise OR on values defined as enum
///
/// Example:
/// ```
/// enum MyFlag : unsigned int { FLAG1 = 0x1, FLAG2 = 0x2, FLAG3 = 0x4 };
///
/// using MyFlags = Flags<MyFlag>;
///
/// MyFlags flags = FLAG1 | FLAG3;
///
/// assert(flags & FLAG1);
/// assert(flags & FLAG3);
/// ```
///
/// @tparam ENUM enum defining the flags
template <typename ENUM>
class Flags {
public:
    static_assert(std::is_enum<ENUM>::value, "Flags is only usable on enumeration types.");

    typedef ENUM enum_type;

    constexpr inline Flags() noexcept : mask(0) {}

    constexpr inline Flags(ENUM flag) noexcept : mask((unsigned int) (flag)) {}

    constexpr inline Flags(unsigned int flags) noexcept : mask(flags) {}

    constexpr inline Flags(std::initializer_list<ENUM> flags) noexcept :
        mask(initializer_list_helper(flags.begin(), flags.end()))
    {
    }

    /// Test if there are set flags
    ///
    /// @return `true` if there are any flags set, `false` otherwise
    constexpr bool
    has_flags() const noexcept
    {
        return this->mask != 0;
    }

    /// Set a flag
    ///
    /// @param rhs Flag to set
    /// @return New flags
    constexpr Flags &
    operator|=(ENUM rhs) noexcept
    {
        this->mask |= rhs;
        return *this;
    }

    /// Add a new flag and return the result
    ///
    /// @param rhs Flag to add
    /// @return New flags
    constexpr Flags
    operator|(ENUM rhs) const noexcept
    {
        Flags<ENUM> flags(this->mask);
        flags |= rhs;
        return flags;
    }

    /// Test if a flag is set
    ///
    /// @param flag Flag to test
    /// @return `true` if the flag `flag` is set, `false` otherwise
    constexpr bool
    operator&(ENUM flag)
    {
        return (this->mask & flag);
    }

private:
    constexpr static inline unsigned int
    initializer_list_helper(typename std::initializer_list<ENUM>::const_iterator it,
                            typename std::initializer_list<ENUM>::const_iterator end) noexcept
    {
        return (it == end ? (unsigned int) 0
                          : ((unsigned int) (*it) | initializer_list_helper(it + 1, end)));
    }

    /// Bit mask iwth flags
    unsigned int mask;
};

template <typename ENUM>
constexpr inline Flags<ENUM>
operator|(ENUM one, ENUM two)
{
    Flags<ENUM> flags(one);
    flags |= two;
    return flags;
}

} // namespace godzilla
