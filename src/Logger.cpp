#include "Logger.h"
#include "CallStack.h"
#include "fmt/printf.h"

namespace godzilla {

Logger::Logger() : num_errors(0), num_warnings(0)
{
    _F_;
}

std::size_t
Logger::get_num_entries() const
{
    _F_;
    return this->entries.size();
}

std::size_t
Logger::get_num_errors() const
{
    _F_;
    return this->num_errors;
}

std::size_t
Logger::get_num_warnings() const
{
    _F_;
    return this->num_warnings;
}

void
Logger::print() const
{
    _F_;
    for (auto & s : this->entries)
        fmt::fprintf(stderr, "%s\n", s.c_str());

    fmt::fprintf(stderr, "%s", (const char *) Terminal::Color::magenta);
    if (this->num_errors > 0)
        fmt::fprintf(stderr, "%zd error(s)", this->num_errors);

    if (this->num_warnings > 0) {
        if (this->num_errors > 0)
            fmt::fprintf(stderr, ", ");

        fmt::fprintf(stderr, "%zd warning(s)", this->num_warnings);
    }
    fmt::fprintf(stderr, " found.%s\n", (const char *) Terminal::Color::normal);
}

} // namespace godzilla
