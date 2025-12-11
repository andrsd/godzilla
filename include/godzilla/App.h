// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "mpicpp-lite/mpicpp-lite.h"
#include "cxxopts/cxxopts.hpp"
#include "godzilla/CallStack.h"
#include "godzilla/Parameters.h"
#include "godzilla/Factory.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Qtr.h"
#include "godzilla/Logger.h"
#include <chrono>
#include <fstream>

namespace mpi = mpicpp_lite;

namespace godzilla {

class Problem;

class App : public PrintInterface {
public:
    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(const mpi::Communicator & comm, const String & name);

    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param registry Registry with classes that will be used by the application
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(const mpi::Communicator & comm, Registry & registry, const String & name);

    virtual ~App();

    /// Get logger associated with the application
    ///
    /// @return Logger
    Logger * get_logger();

    /// Get Application name
    ///
    /// @return Application name
    const String & get_name() const;

    /// Get application version
    ///
    /// @return The application version as a string
    virtual const String & get_version() const;

    /// Get the instance of registry
    ///
    /// @return Instance of the Registry class
    Registry & get_registry();

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

    /// Check integrity of the application
    ///
    /// @return `true` if the check passed, `false` otherwise
    bool check_integrity();

    /// Run the application
    ///
    /// @return Exit code
    virtual int run();

    /// Get level of verbosity
    ///
    /// @return The verbosity level
    const unsigned int & get_verbosity_level() const;

    /// Set verbosity level
    ///
    /// @param level Verbosity level
    void set_verbosity_level(unsigned int level);

    /// Get restart file name
    ///
    /// @return The restart file name
    const String & get_restart_file_name() const;

    /// Set restart file name
    ///
    /// @file_name The restart file name
    void set_restart_file_name(const String & file_name);

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    const mpi::Communicator & get_comm() const;

    /// Get parameters for a class
    ///
    /// @param class_name Class name to get parameters for
    /// @return Parameters for class `class_name`
    Parameters * get_parameters(const String & class_name);

    /// Build object using the Factory
    ///
    /// This is convenience API for building object with having the `_app` parameter set to this
    /// application object.
    ///
    /// @param obj_name Name of the object
    /// @param parameters Input parameters
    /// @return The constructed object
    template <typename T>
    T * build_object(const String & obj_name, Parameters & parameters);

    template <typename T>
    T * build_object(const String & obj_name, Parameters * parameters);

    /// Export parameters into a YAML format
    void export_parameters_yaml() const;

    /// Set file name where to wrote the perf log
    ///
    /// @param file_name Perf log file name
    void set_perf_log_file_name(const String & file_name);

    /// Redirect standard output into file
    ///
    /// @param file_name File to redirect stdout to
    void redirect_stdout(const String & file_name);

    /// Redirect standard error into file
    ///
    /// @param file_name File to redirect stderr to
    void redirect_stderr(const String & file_name);

protected:
    /// Run the problem build via `build_from_yml`
    void run_problem();

    /// Write performance log
    ///
    /// @param file_name File name to write into
    /// @param run_time Total application run time
    void write_perf_log(const String file_name, std::chrono::duration<double> run_time) const;

private:
    /// Application name
    String name;
    /// MPI communicator
    mpi::Communicator mpi_comm;
    /// Registry
    Registry & registry;
    /// Log with errors and/or warnings
    Qtr<Logger> logger;
    /// Verbosity level
    unsigned int verbosity_level;
    /// Restart file name
    String restart_file_name;
    /// Performance log file name
    String perf_log_file_name;
    /// File stream for redirected stdout.
    std::ofstream stdout_file_;
    /// Stream buffer for redirected stdout.
    std::streambuf * cout_buf_;
    /// File stream for redirected stderr.
    std::ofstream stderr_file_;
    /// Stream buffer for redirected stderr.
    std::streambuf * cerr_buf_;
    /// Pointer to `Problem`
    Problem * problem;
    /// Factory for building objects
    Factory factory;

public:
    static void register_objects(Registry & registry);
};

template <typename T>
T *
App::build_object(const String & obj_name, Parameters & parameters)
{
    parameters.set<App *>("app", this);
    return this->factory.create<T>(obj_name, parameters);
}

template <typename T>
T *
App::build_object(const String & obj_name, Parameters * parameters)
{
    parameters->set<App *>("app", this);
    return this->factory.create<T>(obj_name, parameters);
}

} // namespace godzilla
