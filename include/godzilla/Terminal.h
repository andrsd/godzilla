// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/String.h"
#include <iostream>
#include <fmt/format.h>

namespace godzilla {

/// Utility class for tracking terminal capabilities (like colors)
///
class Terminal {
public:
    /// Terminal code as a class
    ///
    /// Classes can be namespaced to avoid name collisions with other packages (like googletest)
    /// We can detect that this is being put into a stream and potentially strip it
    struct Code {
        explicit Code(const char * code);
        operator const String &() const; // NOLINT(google-explicit-constructor)
        operator const char *() const; // NOLINT(google-explicit-constructor)

    private:
        String str;
    };

    /// Terminal color is a special code
    struct Color : public Code {
        explicit Color(const char * code);
    };

private:
    /// Number of colors supported by the terminal
    static unsigned int num_colors;

public:
    /// Query if terminal has colors
    ///
    /// @return true if terminal supports colors
    static bool has_colors();

    /// Set the terminal to use colors
    static void set_colors(bool state);

    // Colors
    static Color black;
    static Color red;
    static Color green;
    static Color yellow;
    static Color blue;
    static Color magenta;
    static Color cyan;
    static Color white;
    static Color normal;

    // Erase functions
    static Code erase_screen;
    static Code erase_line;
    static Code erase_ln_to_cursor;
    static Code erase_ln_from_cursor;
};

} // namespace godzilla

/// Operator to print the color to the terminal
std::ostream & operator<<(std::ostream & os, const godzilla::Terminal::Color & clr);
std::ostream & operator<<(std::ostream & os, const godzilla::Terminal::Code & clr);

template <>
struct fmt::formatter<godzilla::Terminal::Code> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const godzilla::Terminal::Code & e, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", (const char *) e);
    }
};

template <>
struct fmt::formatter<godzilla::Terminal::Color> {
    constexpr auto
    parse(fmt::format_parse_context & ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const godzilla::Terminal::Color & e, FormatContext & ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", (const char *) e);
    }
};
