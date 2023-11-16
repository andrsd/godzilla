#pragma once

#include "mpicpp-lite/mpicpp-lite.h"
#include "cxxopts/cxxopts.hpp"
#include "godzilla/Parameters.h"
#include "godzilla/Factory.h"
#include "godzilla/PrintInterface.h"

namespace godzilla {

namespace mpi = mpicpp_lite;

class Problem;
class InputFile;
class Logger;

class App : public PrintInterface {
public:
    /// Build and application object
    ///
    /// @param comm MPI communicator
    /// @param app_name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(const mpi::Communicator & comm,
        const std::string & app_name = "",
        int argc = 0,
        const char * const * argv = nullptr);
    virtual ~App();

    Logger * get_logger() const;

    /// Get Application name
    ///
    /// @return Application name
    [[deprecated("Use name() instead")]] const std::string & get_name() const;

    const std::string & name() const;

    /// Get application version
    ///
    /// @return The application version as a string
    [[deprecated("Use version() instead")]] virtual const std::string & get_version() const;

    virtual const std::string & version() const;

    /// Get the factory for building objects
    ///
    /// @return Factory that builds objects
    Factory & factory();

    /// Get pointer to the `Problem` class in this application
    ///
    /// @return Get problem this application is representing
    virtual Problem * get_problem() const;

    /// Parse command line arguments
    ///
    /// @return Result of parsing the command line
    virtual cxxopts::ParseResult parse_command_line();

    /// Process command line
    ///
    /// @param result Result from calling `parse_command_line` or `cxxopt::parse`
    virtual void process_command_line(cxxopts::ParseResult & result);

    /// Run the application
    virtual void run();

    /// Get level of verbosity
    ///
    /// @return The verbosity level
    [[deprecated("Use verbosity_level() instead.")]] virtual const unsigned int &
    get_verbosity_level() const;

    virtual const unsigned int & verbosity_level() const;

    /// Get the input file name
    ///
    /// @return The input file name
    virtual const std::string & get_input_file_name() const;

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    [[deprecated("Use comm() instead")]] virtual const mpi::Communicator & get_comm() const;

    virtual const mpi::Communicator & comm() const;

    /// Get parameters for a class
    ///
    /// @param class_name Class name to get parameters for
    /// @return Parameters for class `class_name`
    Parameters * get_parameters(const std::string & class_name);

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

    /// Create command line options
    ///
    virtual void create_command_line_options();

    /// Build application objects from an input file
    ///
    /// @param file_name The input file name
    virtual void build_from_yml(const std::string & file_name);

    /// Check integrity of the application
    virtual void check_integrity();

    /// Run the input file
    ///
    /// @param input_file_name Input file name
    virtual void run_input_file(const std::string & input_file_name);

    /// Run the problem build via `build_from_yml`
    virtual void run_problem();

    /// Application name
    std::string _name;

    /// MPI communicator
    mpi::Communicator _comm;

    /// Log with errors and/or warnings
    Logger * log;

    /// The number of command line arguments
    int argc;

    /// The command line arguments
    const char * const * argv;

    /// Command line options
    cxxopts::Options cmdln_opts;

    /// Verbosity level
    unsigned int _verbosity_level;

    /// YML file with application objects
    InputFile * yml;

    /// Faactory for building objects
    Factory _factory;
};

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters & parameters)
{
    parameters.set<App *>("_app") = this;
    return this->_factory.create<T>(class_name, obj_name, parameters);
}

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters * parameters)
{
    parameters->set<App *>("_app") = this;
    return this->_factory.create<T>(class_name, obj_name, parameters);
}

} // namespace godzilla
