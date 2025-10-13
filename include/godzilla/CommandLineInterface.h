// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "mpicpp-lite/mpicpp-lite.h"
#include "cxxopts/cxxopts.hpp"
#include "godzilla/PrintInterface.h"
#include "godzilla/Qtr.h"

namespace godzilla {

class Problem;
class InputFile;

class CommandLineInterface {
public:
    /// Build an application object
    ///
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    CommandLineInterface(int argc, const char * const * argv, const std::string & app_name = "");

    /// Build an application
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param args Command line arguments (without the executable name as first argument)
    CommandLineInterface(const std::vector<std::string> & args, const std::string & app_name = "");

    virtual ~CommandLineInterface();

    /// Get input file
    ///
    /// @return The input file
    const InputFile & get_input_file() const;

    /// Parse command line arguments
    ///
    /// @return Result of parsing the command line
    virtual cxxopts::ParseResult parse_command_line(cxxopts::Options & opts);

    /// Get the input file name
    ///
    /// @return The input file name
    const std::string & get_input_file_name() const;

    /// Get command line options
    ///
    /// @return Command line options
    cxxopts::Options & get_command_line_opts();

    /// Create command line options
    ///
    virtual cxxopts::Options create_command_line_options();

    /// Run the input file
    ///
    /// @param input_file_name Input file name
    void run_input_file(const std::string & input_file_name);

    /// Run the problem build via `build_from_yml`
    void run_problem();

    int run();

private:
    /// Create an input file instance
    virtual Qtr<InputFile> create_input_file();

    /// Process command line
    ///
    /// @param result Result from calling `parse_command_line` or `cxxopt::parse`
    virtual void process_command_line(cxxopts::Options & opts, const cxxopts::ParseResult & result);

    /// Command line arguments
    std::vector<std::string> args;

    /// Command line options
    cxxopts::Options cmdln_opts;

    /// YML file with application objects
    Qtr<InputFile> yml;
};

} // namespace godzilla
