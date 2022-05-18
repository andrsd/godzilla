#pragma once

#include <string>
#include "Error.h"
#include "petscsys.h"

namespace godzilla {

class Object;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    PrintInterface(const Object * obj);

protected:
    /// Print a message on a terminal
    ///
    /// @param level Verbosity level. If application verbose level is higher than this number, the
    ///              message will be printed.
    /// @param format String specifying how to interpret the data
    /// @param ... Arguments specifying data to print
    template <typename... Args>
    void
    godzilla_print(unsigned int level, const char * format, Args &&... args) const
    {
        if (level <= this->verbosity_level && this->proc_id == 0) {
            internal::fprintf(std::cout, "%s: ", this->prefix);
            internal::fprintf(std::cout, format, std::forward<Args>(args)...);
            internal::fprintf(std::cout, "\n");
        }
    }

private:
    /// Processor ID
    const PetscMPIInt & proc_id;
    /// Verbosity level
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

} // namespace godzilla
