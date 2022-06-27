#pragma once

#include <string>
#include "Error.h"
#include "petscsys.h"
#include "fmt/printf.h"

namespace godzilla {

class Object;
class App;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    PrintInterface(const Object * obj);
    PrintInterface(const App * app);
    PrintInterface(MPI_Comm comm, const unsigned int & verbosity_level, const std::string & prefix);

protected:
    /// Print a message on a terminal
    ///
    /// @param level Verbosity level. If application verbose level is higher than this number, the
    ///              message will be printed.
    /// @param format String specifying how to interpret the data
    /// @param ... Arguments specifying data to print
    template <typename... Args>
    void
    lprintf(unsigned int level, const char * format, Args &&... args) const
    {
        if (level <= this->verbosity_level && this->proc_id == 0) {
            fmt::fprintf(stdout, "%s: ", this->prefix);
            fmt::fprintf(stdout, format, std::forward<Args>(args)...);
            fmt::fprintf(stdout, "\n");
        }
    }

private:
    /// Processor ID
    PetscMPIInt proc_id;
    /// Verbosity level
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

} // namespace godzilla
