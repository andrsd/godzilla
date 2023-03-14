#pragma once

#include <string>
#include "fmt/printf.h"

namespace godzilla {

namespace mpi {
class Communicator;
}

class Object;
class App;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    explicit PrintInterface(const Object * obj);
    explicit PrintInterface(const App * app);
    PrintInterface(const mpi::Communicator & comm,
                   const unsigned int & verbosity_level,
                   std::string prefix);

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
            fmt::fprintf(stdout, "%s: ", this->prefix.c_str());
            fmt::fprintf(stdout, format, std::forward<Args>(args)...);
            fmt::fprintf(stdout, "\n");
        }
    }

private:
    /// Processor ID
    int proc_id;
    /// Verbosity level
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

} // namespace godzilla
