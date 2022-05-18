#pragma once

#include <iostream>

namespace godzilla {

/// Utility class for tracking terminal capabilities (like colors)
///
class Terminal {
public:
    /// Terminal color as a class
    ///
    /// Classes can be namespaced to avoid name collisions with other packages (like googletest)
    /// We can detect that this is being put into a stream and potentially strip it
    ///
    /// @param aclr Control characters representing the color
    struct Color {
        Color(const char * aclr) : str(aclr) {}

        const char * str;

    public:
        static Color black;
        static Color red;
        static Color green;
        static Color yellow;
        static Color blue;
        static Color magenta;
        static Color cyan;
        static Color white;
        static Color normal;
    };

    /// Query if terminal has colors
    ///
    /// @return true if terminal supports colors
    static bool has_colors();

    /// Number of colors supported by the terminal
    static unsigned int num_colors;
};

} // namespace godzilla

/// Operator to print the color to the terminal
std::ostream & operator<<(std::ostream & os, const godzilla::Terminal::Color & clr);
