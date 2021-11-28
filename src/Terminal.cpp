#include "Terminal.h"

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
Terminal::hasColors()
{
    return num_colors > 1;
}

unsigned int Terminal::num_colors = 256;

} // namespace godzilla

std::ostream &
operator<<(std::ostream & os, const godzilla::Terminal::Color & clr)
{
    if (godzilla::Terminal::hasColors())
        os << clr.str;
    return os;
}
