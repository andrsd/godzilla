#pragma once

#include <string>
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
    void godzilla_print(unsigned int level, const char * format, ...) const;

private:
    /// Processor ID
    const PetscMPIInt & proc_id;
    /// Verbosity level
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

} // namespace godzilla
