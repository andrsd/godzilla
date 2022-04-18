#pragma once

#include "mpi.h"
#include "PrintInterface.h"
#include "CmdLineArgParser.h"
#include "InputParameters.h"
#include "Factory.h"
#include "Logger.h"

namespace godzilla {

class Function;
class Grid;
class Problem;

class App : public PrintInterface {
public:
    /// Build and application object
    ///
    /// @param app_name Name of the application
    /// @param comm MPI communicator
    App(const std::string & app_name, MPI_Comm comm);
    virtual ~App();

    const Logger & getLogger() const;

    /// Create method can be used to additional object allocation, etc. needed before the
    /// application runs
    virtual void create();

    /// Parse command line arguments
    ///
    /// @param argc Number of command line arguments
    /// @param argv Command line argumnts
    virtual void parseCommandLine(int argc, char * argv[]);

    /// Run the application
    ///
    /// This is the main entry point
    virtual void run();

    /// Get level of verbosity
    ///
    /// @return The verbosity level
    virtual const unsigned int & getVerbosityLevel() const;

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    virtual const MPI_Comm & getComm() const;

    /// Get communicator rank
    ///
    /// @return The rank of the calling process in the application communicator
    virtual const PetscMPIInt & getCommRank() const;

    /// Get communicator size
    ///
    /// @return Size of the group associated with the application communicator
    virtual const PetscMPIInt & getCommSize() const;

    /// Build object using the Factory
    ///
    /// This is convenience API for building object with having the `_app` parameter set to this
    /// application object.
    ///
    /// @param class_name C++ class name of the object to build
    /// @param name Name of the object
    /// @param parameters Input parameters
    /// @return The constructed object
    template <typename T>
    T * buildObject(const std::string & class_name,
                    const std::string & name,
                    InputParameters & parameters);

protected:
    /// Build application objects from a GYML file
    ///
    /// @param file_name The GYML file name
    virtual void buildFromGYML(const std::string & file_name);

    /// Check integrity of the application
    virtual void checkIntegrity();

    /// Run the input file
    ///
    /// This is the method that will be called wehn user specify -i command line parameter
    ///
    /// @param file_name The name of the file specified via `-i` parameter
    virtual void runInputFile(const std::string & file_name);

    /// Run the problem build via `buildFromGYML`
    virtual void runProblem();

    /// MPI communicators
    MPI_Comm comm;

    /// Size of the MPI communicator
    PetscMPIInt comm_size;

    /// MPI rank (aka processor ID) of this application
    PetscMPIInt comm_rank;

    /// Log with errors and/or warnings
    Logger log;

    /// Command line parser
    CmdLineArgParser args;

    /// Command line argument passed via `-i` parameter
    TCLAP::ValueArg<std::string> input_file_arg;

    /// Command line argument passed via `--verbose` parameter
    TCLAP::ValueArg<unsigned int> verbose_arg;

    /// Command line switch to turn terminal colors off (passed via `--no-colors`)
    TCLAP::SwitchArg no_colors_switch;

    /// Verbosity level
    unsigned int verbosity_level;

    /// Grid object
    Grid * grid;

    /// Problem object
    Problem * problem;

    /// List of functions within this application
    std::vector<Function *> functions;

    friend class FunctionInterface;
};

template <typename T>
T *
App::buildObject(const std::string & class_name,
                 const std::string & name,
                 InputParameters & parameters)
{
    parameters.set<const App *>("_app") = this;
    return Factory::create<T>(class_name, name, parameters);
}

} // namespace godzilla
