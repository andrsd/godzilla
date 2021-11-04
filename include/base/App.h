#pragma once

#include <mpi.h>
#include "base/GPrintInterface.h"
#include "input/CmdLineArgParser.h"

namespace godzilla {

class GGrid;
class GProblem;
class GExecutioner;


class App : public GPrintInterface
{
public:
    App(const std::string & app_name, MPI_Comm comm);
    ///
    virtual void create();
    /// Parse command line arguments
    virtual void parseCommandLine(int argc, char *argv[]);

    ///
    virtual void run();
    /// Get level of verbosity
    virtual const unsigned int & getVerbosityLevel() const;

protected:
    ///
    virtual void buildFromGYML(const std::string & file_name);
    /// Run the input file
    virtual void runInputFile(const std::string & file_name);
    /// Start the executioner
    virtual void startExecutioner();

    MPI_Comm comm;

    CmdLineArgParser args;
    TCLAP::ValueArg<std::string> input_file_arg;
    TCLAP::ValueArg<unsigned int> verbose_arg;

    /// verbosity level
    unsigned int verbosity_level;

    /// Grid
    GGrid * grid;
    /// Problem
    GProblem * problem;
    /// Executioner
    GExecutioner * executioner;
};

}
