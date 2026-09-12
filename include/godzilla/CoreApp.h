// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Parameters.h"
#include "godzilla/Registry.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Qtr.h"
#include "godzilla/Ref.h"
#include "godzilla/Logger.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include <chrono>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace mpi = mpicpp_lite;

namespace godzilla {

class CoreApp : public PrintInterface {
public:
    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    CoreApp(mpi::Communicator comm, String name);

    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param registry Registry with classes that will be used by the application
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    CoreApp(mpi::Communicator comm, Registry & registry, String name);

    virtual ~CoreApp();

    /// Get logger associated with the application
    ///
    /// @return Logger
    Ref<Logger> get_logger();

    /// Get Application name
    ///
    /// @return Application name
    String get_name() const;

    /// Get application version
    ///
    /// @return The application version as a string
    virtual String get_version() const;

    /// Get the instance of registry
    ///
    /// @return Instance of the Registry class
    Registry & get_registry();

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
    fs::path get_restart_file_name() const;

    /// Set restart file name
    ///
    /// @file_name The restart file name
    void set_restart_file_name(fs::path file_name);

    /// Get MPI communicator
    ///
    /// @return MPI communicator
    mpi::Communicator get_comm() const;

    /// Create parameters for type T
    ///
    /// @tparam C++ object that provides `parameters()`
    /// @return Parameters for class T
    template <typename T>
        requires requires {
            { T::parameters() } -> std::same_as<Parameters>;
        }
    Parameters
    make_parameters()
    {
        static_assert(IsConstructibleFromParams<T>::value,
                      "T must be constructible from `const Parameters &`");

        auto pars = T::parameters();
        pars.template set<String>("_type", utils::type_name<T>());
        pars.template set<Ref<CoreApp>>("app", ref(*this));
        return pars;
    }

    /// Export parameters into a YAML format
    void export_parameters_yaml() const;

    /// Set file name where to wrote the perf log
    ///
    /// @param file_name Perf log file name
    void set_perf_log_file_name(fs::path file_name);

    /// Redirect standard output into file
    ///
    /// @param file_name File to redirect stdout to
    void redirect_stdout(fs::path file_name);

    /// Redirect standard error into file
    ///
    /// @param file_name File to redirect stderr to
    void redirect_stderr(fs::path file_name);

protected:
    /// Write performance log
    ///
    /// @param run_time Total application run time
    void write_perf_log(std::chrono::duration<double> run_time) const;

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
    fs::path restart_file_name;
    /// Performance log file name
    fs::path perf_log_file_name;
    /// File stream for redirected stdout.
    std::ofstream stdout_file_;
    /// Stream buffer for redirected stdout.
    std::streambuf * cout_buf_;
    /// File stream for redirected stderr.
    std::ofstream stderr_file_;
    /// Stream buffer for redirected stderr.
    std::streambuf * cerr_buf_;

public:
    static void register_objects(Registry & registry);
};

} // namespace godzilla
