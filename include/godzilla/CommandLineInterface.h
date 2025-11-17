// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "mpicpp-lite/mpicpp-lite.h"
#include "cxxopts/cxxopts.hpp"

namespace godzilla {

class Problem;
class App;

/// Class to provde basic interaction via command line parameters
///
/// NOTE: uses cxxopts internally, but cxxopt objects are passed via API
class CommandLineInterface {
public:
    /// Build an application object
    ///
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    CommandLineInterface(App & app, int argc, const char * const * argv);

    /// Build an application
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param args Command line arguments (without the executable name as first argument)
    CommandLineInterface(App & app, const std::vector<std::string> & args);

    virtual ~CommandLineInterface() = default;

    /// Parse command line arguments
    ///
    /// @return Result of parsing the command line
    virtual cxxopts::ParseResult parse_command_line(cxxopts::Options & opts);

    /// Entry point into command line parameter processing
    ///
    /// @return Process exit code. This can be reported back into calling process if needed.
    int run();

protected:
    std::string get_app_name() const;

    /// Create command line options
    ///
    virtual cxxopts::Options create_command_line_options();

    /// Process command line
    ///
    /// @param result Result from calling `parse_command_line` or `cxxopt::parse`
    virtual void process_command_line(cxxopts::Options & opts, const cxxopts::ParseResult & result);

private:
    /// Application we are part of
    App & app;
    /// Command line arguments
    std::vector<std::string> args;
};

} // namespace godzilla
