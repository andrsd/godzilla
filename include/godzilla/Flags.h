// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <type_traits>
#include <initializer_list>
#include <concepts>

namespace godzilla {

template <typename T>
concept IsEnum = std::is_enum_v<T>;

template <typename ENUM>
struct IsFlagEnum : std::false_type {};

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
template <IsEnum ENUM>
class Flags {
public:
    using UnderlyingType = std::underlying_type_t<ENUM>;

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
        this->mask |= static_cast<UnderlyingType>(rhs);
        return *this;
    }

    /// Set flags
    ///
    /// @param rhs Flags to set
    /// @return New flags
    constexpr Flags &
    operator|=(Flags rhs)
    {
        this->mask |= rhs.mask;
        return *this;
    }

    /// Add a new flag and return the result
    ///
    /// @param rhs Flag to add
    /// @return New flags
    constexpr Flags
    operator|(ENUM rhs) const noexcept
    {
        return Flags(this->mask | static_cast<UnderlyingType>(rhs));
        //   Flags<ENUM> flags(this->mask);
        // flags |= rhs;
        // return flags;
    }

    /// Add flags
    ///
    /// @param rhs Flags to set
    /// @return New flags
    constexpr Flags
    operator|(Flags rhs) const
    {
        return Flags(this->mask | this->mask);
    }

    /// Test if a flag is set
    ///
    /// @param flag Flag to test
    /// @return `true` if the flag `flag` is set, `false` otherwise
    constexpr bool
    operator&(ENUM flag) const noexcept
    {
        return (this->mask & static_cast<UnderlyingType>(flag)) != 0;
    }

    [[nodiscard]] constexpr UnderlyingType
    get_mask() const noexcept
    {
        return mask;
    }

private:
    constexpr static inline UnderlyingType
    initializer_list_helper(typename std::initializer_list<ENUM>::const_iterator it,
                            typename std::initializer_list<ENUM>::const_iterator end) noexcept
    {
        return (it == end ? (UnderlyingType) 0
                          : ((UnderlyingType) (*it) | initializer_list_helper(it + 1, end)));
    }

    /// Bit mask with flags
    UnderlyingType mask;
};

template <IsEnum ENUM>
    requires(IsFlagEnum<ENUM>::value)
constexpr Flags<ENUM>
operator|(ENUM lhs, ENUM rhs)
{
    using U = std::underlying_type_t<ENUM>;
    return Flags<ENUM>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template <IsEnum ENUM>
    requires(IsFlagEnum<ENUM>::value)
constexpr Flags<ENUM>
operator|(Flags<ENUM> lhs, ENUM rhs)
{
    return lhs | Flags<ENUM>(rhs);
}

template <IsEnum ENUM>
    requires(IsFlagEnum<ENUM>::value)
constexpr Flags<ENUM>
operator|(ENUM lhs, Flags<ENUM> rhs)
{
    return Flags<ENUM>(lhs) | rhs;
}

} // namespace godzilla
