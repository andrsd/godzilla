#include "input/CmdLineArgParser.h"


namespace godzilla {

CmdLineArgParser::CmdLineArgParser(const std::string & prog_name, const std::string & version) :
    cmd(prog_name, version)
{
}

void
CmdLineArgParser::add(TCLAP::Arg &a)
{
    try {
        cmd.add(a);
    }
    catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

void
CmdLineArgParser::parse(int argc, const char * const * argv)
{
    try {
        cmd.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}

}
