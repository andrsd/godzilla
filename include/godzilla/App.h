// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

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
    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(const mpi::Communicator & comm,
        const std::string & name,
        int argc,
        const char * const * argv);

    /// Build an application
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param args Command line arguments (without the executable name as first argument)
    App(const mpi::Communicator & comm,
        const std::string & name,
        const std::vector<std::string> & args);

    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param registry Registry with classes that will be used by the application
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(const mpi::Communicator & comm,
        const Registry & registry,
        const std::string & name,
        int argc,
        const char * const * argv);

    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param registry Registry with classes that will be used by the application
    /// @param name Name of the application
    /// @param args Command line arguments (without the executable name as first argument)
    App(const mpi::Communicator & comm,
        const Registry & registry,
        const std::string & name,
        const std::vector<std::string> & args);

    virtual ~App();

    /// Get logger associated with the application
    ///
    /// @return Logger
    Logger * get_logger() const;

    /// Get Application name
    ///
    /// @return Application name
    const std::string & get_name() const;

    /// Get application version
    ///
    /// @return The application version as a string
    virtual const std::string & get_version() const;

    /// Get the factory for building objects
    ///
    /// @return Factory that builds objects
    Factory & get_factory();

    /// Get pointer to the `Problem` class in this application
    ///
    /// @return Get problem this application is representing
    Problem * get_problem() const;

    /// Get pointer to the `Problem`-derived class in this application
    ///
    /// @return The problem this application is solving
    template <typename T>
    T *
    get_problem() const
    {
        CALL_STACK_MSG();
        return dynamic_cast<T *>(this->problem);
    }

    void set_problem(Problem * problem);

    /// Parse command line arguments
    ///
    /// @return Result of parsing the command line
    virtual cxxopts::ParseResult parse_command_line();

    /// Process command line
    ///
    /// @param result Result from calling `parse_command_line` or `cxxopt::parse`
    virtual void process_command_line(const cxxopts::ParseResult & result);

    /// Check integrity of the application
    ///
    /// @return `true` if the check passed, `false` otherwise
    bool check_integrity();

    /// Run the application
    virtual void run();

    /// Get level of verbosity
    ///
    /// @return The verbosity level
    const unsigned int & get_verbosity_level() const;

    /// Set verbosity level
    ///
    /// @param level Verbosity level
    void set_verbosity_level(unsigned int level);

    /// Get the input file name
    ///
    /// @return The input file name
    const std::string & get_input_file_name() const;

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    const mpi::Communicator & get_comm() const;

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
    /// Get command line options
    ///
    /// @return Command line options
    cxxopts::Options & get_command_line_opts();

    /// Set the input file
    ///
    /// @param input_file Input file to set
    void set_input_file(InputFile * input_file);

    /// Create command line options
    ///
    virtual void create_command_line_options();

    /// Create an input file instance
    virtual InputFile * create_input_file();

    /// Build application objects from an input file
    ///
    /// @param file_name The input file name
    void build_from_yml(const std::string & file_name);

    /// Run the input file
    ///
    /// @param input_file_name Input file name
    void run_input_file(const std::string & input_file_name);

    /// Run the problem build via `build_from_yml`
    void run_problem();

private:
    /// Application name
    std::string name;

    /// MPI communicator
    mpi::Communicator mpi_comm;

    /// Log with errors and/or warnings
    Logger * logger;

    /// Command line arguments
    std::vector<std::string> args;

    /// Command line options
    cxxopts::Options cmdln_opts;

    /// Verbosity level
    unsigned int verbosity_level;

    /// YML file with application objects
    InputFile * yml;

    /// Pointer to `Problem`
    Problem * problem;

    /// Factory for building objects
    Factory factory;

public:
    /// Get the instance of registry
    ///
    /// @return Instance of the Registry class
    static Registry & get_registry();

    static void registerObjects(Registry & registry);

private:
    static Registry registry;
};

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters & parameters)
{
    parameters.set<App *>("_app") = this;
    return this->factory.create<T>(class_name, obj_name, parameters);
}

template <typename T>
T *
App::build_object(const std::string & class_name,
                  const std::string & obj_name,
                  Parameters * parameters)
{
    parameters->set<App *>("_app") = this;
    return this->factory.create<T>(class_name, obj_name, parameters);
}

} // namespace godzilla
