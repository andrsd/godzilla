#pragma once

#include "tclap/CmdLine.h"

namespace godzilla {

/// Wrapper class around [TCLAP](https://sourceforge.net/projects/tclap/) (Templated Command Line
/// Argument Parser)
///
class CmdLineArgParser {
public:
    /// Construct a command line parser object
    ///
    /// @param prog_name The name of the binary (shown when printing usage)
    /// @param version The version of the binary
    explicit CmdLineArgParser(const std::string & prog_name, const std::string & version = "none");

    /// Adds an argument
    ///
    /// @param a Comand line parser argument
    void add(TCLAP::Arg & a);

    /// Parse the command line
    ///
    /// @param argc The number of arguments
    /// @param argv The command line parameters
    void parse(int argc, const char * const * argv);

protected:
    /// TCALP command line object
    TCLAP::CmdLine cmd;
};

} // namespace godzilla
