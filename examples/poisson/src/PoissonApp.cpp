// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "PoissonApp.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/CommandLineInterface.h"

PoissonApp::PoissonApp(mpi::Communicator comm, const std::string & name, int argc, char * argv[]) :
    godzilla::App(comm, name),
    godzilla::CommandLineInterface(argc, argv, name)
{
    CALL_STACK_MSG();
}

cxxopts::Options
PoissonApp::create_command_line_options()
{
    CALL_STACK_MSG();
    cxxopts::Options opts(get_name());
    opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    opts.add_option("", "d", "dimension", "Dimension of the problem", cxxopts::value<int>(), "");
    return opts;
}

void
PoissonApp::process_command_line(cxxopts::Options & opts, const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();
    if (result.count("help")) {
        fmt::print("{}", opts.help());
    }
    else if (result.count("dimension")) {
        auto dim = result["dimension"].as<int>();
        fmt::println("dim = {}", dim);
    }
}

int
PoissonApp::run()
{
    CALL_STACK_MSG();
    return godzilla::CommandLineInterface::run();
}
