// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Terminal.h"

namespace godzilla {

Terminal::Color Terminal::Color::black("\33[30m");
Terminal::Color Terminal::Color::red("\33[31m");
Terminal::Color Terminal::Color::green("\33[32m");
Terminal::Color Terminal::Color::yellow("\33[33m");
Terminal::Color Terminal::Color::blue("\33[34m");
Terminal::Color Terminal::Color::magenta("\33[35m");
Terminal::Color Terminal::Color::cyan("\33[36m");
Terminal::Color Terminal::Color::white("\33[37m");
Terminal::Color Terminal::Color::normal("\33[39m");

bool
Terminal::has_colors()
{
    return num_colors > 1;
}

unsigned int Terminal::num_colors = 256;

} // namespace godzilla

std::ostream &
operator<<(std::ostream & os, const godzilla::Terminal::Color & clr)
{
    os << static_cast<const char *>(clr);
    return os;
}
