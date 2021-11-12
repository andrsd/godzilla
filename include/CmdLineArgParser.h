#pragma once

#include "tclap/CmdLine.h"

namespace godzilla {

/// Wrapper class around [TCLAP](https://sourceforge.net/projects/tclap/) (Templated Command Line
/// Argument Parser)
///
///
class CmdLineArgParser {
public:
    CmdLineArgParser(const std::string & prog_name, const std::string & version = "none");

    void add(TCLAP::Arg & a);

    void parse(int argc, const char * const * argv);

protected:
    TCLAP::CmdLine cmd;
};

} // namespace godzilla
