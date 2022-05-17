#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "petsc.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

class App;
class Object;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    PrintInterface(const App & app);
    PrintInterface(const Object * obj);

protected:
    /// Print a message on a terminal
    template <typename... Args>
    void
    godzilla_print(unsigned int level, Args &&... args) const
    {
        if (level <= this->verbosity_level) {
            std::ostringstream oss;
            internal::godzilla_stream_all(oss, this->prefix, std::forward<Args>(args)...);
            internal::godzilla_msg_raw(oss.str());
        }
    }

private:
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
    ///
    const MPI_Comm & pi_comm;
};

} // namespace godzilla
