#pragma once

#include <mpi.h>
#include "GPrintInterface.h"
#include "CmdLineArgParser.h"

namespace godzilla {

class GGrid;
class Problem;
class GExecutioner;


class App : public GPrintInterface
{
public:
    App(const std::string & app_name, MPI_Comm comm);
    virtual ~App();
    ///
    virtual void create();
    /// Parse command line arguments
    virtual void parseCommandLine(int argc, char *argv[]);

    ///
    virtual void run();
    /// Get level of verbosity
    virtual const unsigned int & getVerbosityLevel() const;

    virtual const MPI_Comm & getComm() const;

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
    Problem * problem;
    /// Executioner
    GExecutioner * executioner;
};

}
