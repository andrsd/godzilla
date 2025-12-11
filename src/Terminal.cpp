// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Terminal.h"

namespace godzilla {

Terminal::Color Terminal::black("\033[30m");
Terminal::Color Terminal::red("\033[31m");
Terminal::Color Terminal::green("\033[32m");
Terminal::Color Terminal::yellow("\033[33m");
Terminal::Color Terminal::blue("\033[34m");
Terminal::Color Terminal::magenta("\033[35m");
Terminal::Color Terminal::cyan("\033[36m");
Terminal::Color Terminal::white("\033[37m");
Terminal::Color Terminal::normal("\033[39m");

Terminal::Code Terminal::erase_screen("\033[2J");
Terminal::Code Terminal::erase_line("\033[2K");
Terminal::Code Terminal::erase_ln_to_cursor("\033[1K");
Terminal::Code Terminal::erase_ln_from_cursor("\033[0K");

Terminal::Code::Code(const char * code) : str(code) {}

Terminal::Code::operator const String &() const
{
    return this->str;
}

Terminal::Code::operator const char *() const
{
    return this->str.c_str();
}

Terminal::Color::Color(const char * code) : Code(code) {}

// Terminal

bool
Terminal::has_colors()
{
    return num_colors > 1;
}

void
Terminal::set_colors(bool state)
{
    if (state)
        num_colors = 256;
    else
        num_colors = 1;
}

unsigned int Terminal::num_colors = 256;

} // namespace godzilla

std::ostream &
operator<<(std::ostream & os, const godzilla::Terminal::Color & clr)
{
    if (godzilla::Terminal::has_colors())
        os << static_cast<const char *>(clr);
    return os;
}

std::ostream &
operator<<(std::ostream & os, const godzilla::Terminal::Code & clr)
{
    os << static_cast<const char *>(clr);
    return os;
}
