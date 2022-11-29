#pragma once

#include "petsc.h"
#include "CmdLineArgParser.h"
#include "Parameters.h"
#include "Factory.h"
#include "Logger.h"
#include "PrintInterface.h"

namespace godzilla {

class Problem;
class InputFile;

class App : public PrintInterface {
public:
    /// Build and application object
    ///
    /// @param app_name Name of the application
    /// @param comm MPI communicator
    App(const std::string & app_name, MPI_Comm comm);
    virtual ~App();

    Logger * get_logger() const;

    /// Get Application name
    ///
    /// @return Application name
    const std::string & get_name() const;

    /// @return Get problem this application is representing
    virtual Problem * get_problem() const;

    /// Parse command line arguments
    ///
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    virtual void parse_command_line(int argc, char * argv[]);

    /// Run the application
    ///
    /// This is the main entry point
    virtual void run();

    /// Get level of verbosity
    ///
    /// @return The verbosity level
    virtual const unsigned int & get_verbosity_level() const;

    /// Get the input file name
    ///
    /// @return The input file name
    virtual const std::string & get_input_file_name() const;

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    virtual const MPI_Comm & get_comm() const;

    /// Get communicator rank
    ///
    /// @return The rank of the calling process in the application communicator
    virtual const PetscMPIInt & get_comm_rank() const;

    /// Get communicator size
    ///
    /// @return Size of the group associated with the application communicator
    virtual const PetscMPIInt & get_comm_size() const;

    /// Build object using the Factory
    ///
    /// This is convenience API for building object with having the `_app` parameter set to this
    /// application object.
    ///
    /// @param class_name C++ class name of the object to build
    /// @param obj_name Name of the object
    /// @param parameters Input parameters
    /// @return The constructed object
    template <typename T>
    T * build_object(const std::string & class_name,
                     const std::string & obj_name,
                     Parameters & parameters);

    template <typename T>
    T * build_object(const std::string & class_name,
                     const std::string & obj_name,
                     Parameters * parameters);

protected:
    /// Builds an InputFile instance
    virtual InputFile * allocate_input_file();

    /// Create method can be used to additional object allocation, etc. needed before the
    /// application runs
    virtual void create();

    /// Build application objects from an input file
    ///
    /// @param file_name The input file name
    virtual void build_from_yml();

    /// Check integrity of the application
    virtual void check_integrity();

    /// Process command line
    virtual void process_command_line();

    /// Run the input file
    ///
    /// This is the method that will be called when user specifies -i command line parameter
    virtual void run_input_file();

    /// Run the problem build via `build_from_yml`
    virtual void run_problem();

    /// Application name
    std::string name;

    /// MPI communicators
    MPI_Comm comm;

    /// Size of the MPI communicator
    PetscMPIInt comm_size;

    /// MPI rank (aka processor ID) of this application
    PetscMPIInt comm_rank;

    /// Log with errors and/or warnings
    Logger * log;

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

    /// Input file name
    std::string input_file_name;

    /// YML file with application objects
    InputFile * yml;
};

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters & parameters)
{
    parameters.set<const App *>("_app") = this;
    return Factory::create<T>(class_name, obj_name, parameters);
}

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters * parameters)
{
    parameters->set<const App *>("_app") = this;
    return Factory::create<T>(class_name, obj_name, parameters);
}

} // namespace godzilla
