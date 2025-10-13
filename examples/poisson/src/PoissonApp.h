// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/App.h"
#include "godzilla/CommandLineInterface.h"

class PoissonApp : public godzilla::App, public godzilla::CommandLineInterface {
public:
    PoissonApp(mpi::Communicator comm, const std::string & name, int argc, char * argv[]);

    int run() override;

private:
    cxxopts::Options create_command_line_options() override;
    void process_command_line(cxxopts::Options & opts,
                              const cxxopts::ParseResult & result) override;

    // cxxopts::ParseResult parse_command_line();
};
