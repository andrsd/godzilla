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

Terminal::Code Terminal::Code::erase_screen("\033[2J");
Terminal::Code Terminal::Code::erase_line("\033[2K");
Terminal::Code Terminal::Code::erase_ln_to_cursor("\033[1K");
Terminal::Code Terminal::Code::erase_ln_from_cursor("\033[0K");

Terminal::Color::Color(const char * aclr) : str(aclr) {}

Terminal::Color::operator const std::string &() const
{
    return this->str;
}

Terminal::Color::operator const char *() const
{
    return this->str.c_str();
}

// Terminal::Code

Terminal::Code::Code(const char * code) : str(code) {}

Terminal::Code::operator const std::string &() const
{
    return this->str;
}

Terminal::Code::operator const char *() const
{
    return this->str.c_str();
}

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
