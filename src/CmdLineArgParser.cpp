#include "CmdLineArgParser.h"
#include "CallStack.h"

namespace godzilla {

CmdLineArgParser::CmdLineArgParser(const std::string & prog_name, const std::string & version) :
    cmd(prog_name, version)
{
    _F_;
}

void
CmdLineArgParser::add(TCLAP::Arg & a)
{
    _F_;
    cmd.add(a);
}

void
CmdLineArgParser::parse(int argc, const char * const * argv)
{
    _F_;
    cmd.parse(argc, argv);
}

} // namespace godzilla
