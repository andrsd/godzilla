#include "Logger.h"
#include "CallStack.h"

namespace godzilla {

Logger::Logger() : num_errors(0), num_warnings(0)
{
    _F_;
}

std::size_t
Logger::getNumEntries() const
{
    _F_;
    return this->entries.size();
}

std::size_t
Logger::getNumErrors() const
{
    _F_;
    return this->num_errors;
}

std::size_t
Logger::getNumWarnings() const
{
    _F_;
    return this->num_warnings;
}

void
Logger::print() const
{
    _F_;
    for (auto & s : this->entries) {
        std::cerr << s << std::endl;
    }

    if (this->num_errors > 0)
        std::cerr << this->num_errors << " error(s)";

    if (this->num_warnings > 0) {
        if (this->num_errors > 0)
            std::cerr << ", ";

        std::cerr << this->num_warnings << " warning(s)";
    }
    std::cerr << " found." << std::endl;
}

} // namespace godzilla
