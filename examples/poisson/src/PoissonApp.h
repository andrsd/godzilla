// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/App.h"
#include "godzilla/CommandLineInterface.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Qtr.h"
#include "godzilla/Mesh.h"

namespace godzilla {
class DiscreteProblemInterface;
}

class PoissonApp : public godzilla::App, public godzilla::CommandLineInterface {
public:
    PoissonApp(mpi::Communicator comm, const std::string & name, int argc, char * argv[]);

    int run() override;

private:
    cxxopts::Options create_command_line_options();
    void solve_problem(godzilla::Int dim);
    godzilla::Qtr<godzilla::Mesh> create_mesh(godzilla::Int dim);
    void create_auxs(godzilla::DiscreteProblemInterface & prob, godzilla::Int dim);
    void create_bcs(godzilla::DiscreteProblemInterface & prob, godzilla::Int dim);

    cxxopts::Options cli_opts;
    cxxopts::ParseResult cli_result;
};
