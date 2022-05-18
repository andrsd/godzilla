#include "Logger.h"
#include "CallStack.h"
#include "petscsys.h"

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
        internal::fprintf(std::cerr, "%s\n", s);

    internal::fprintf(std::cerr, Terminal::color::red);
    if (this->num_errors > 0)
        internal::fprintf(std::cerr, "%d error(s)", this->num_errors);

    if (this->num_warnings > 0) {
        if (this->num_errors > 0)
            internal::fprintf(std::cerr, ", ");

        internal::fprintf(std::cerr, "%d warning(s)", this->num_warnings);
    }
    internal::fprintf(std::cerr, " found.\n");
    internal::fprintf(std::cerr, Terminal::color::normal);
}

} // namespace godzilla
