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
    template <typename... T>
    void
    lprintf(unsigned int level, fmt::format_string<T...> format, T... args) const
    {
        if (level <= this->verbosity_level && this->proc_id == 0) {
            fmt::print(stdout, "{}: ", this->prefix);
            fmt::print(stdout, format, std::forward<T>(args)...);
            fmt::print(stdout, "\n");
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
