// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "cxxopts/cxxopts.hpp"
#include "fmt/printf.h"
#include "godzilla/Exception.h"
#include "godzilla/Init.h"
#include "godzilla/App.h"
#include "godzilla/String.h"
#include "godzilla/CommandLineInterface.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/FileMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/ExodusIIOutput.h"
#include "mpicpp-lite/mpicpp-lite.h"

using namespace godzilla;

const String version = "1.0";

mpi::Communicator comm_world;

class MeshPartApp : public App, public CommandLineInterface {
public:
    MeshPartApp(int argc, const char * const * argv);

    String
    get_version() const override
    {
        return version;
    }

    int run() override;

protected:
    cxxopts::Options create_command_line_options();
    cxxopts::ParseResult parse(cxxopts::Options & opts);
    Qtr<UnstructuredMesh> load_mesh(const std::string & file_name);
    void partition_mesh_file(const std::string & mesh_file_name);
    void save_partition(UnstructuredMesh * mesh, const std::string & file_name);

    cxxopts::Options cli_opts;
    cxxopts::ParseResult cli_result;
};

MeshPartApp::MeshPartApp(int argc, const char * const * argv) :
    App(comm_world, "mesh-part"),
    CommandLineInterface(*this, argc, argv),
    cli_opts(create_command_line_options()),
    cli_result(parse(this->cli_opts))
{
}

cxxopts::Options
MeshPartApp::create_command_line_options()
{
    cxxopts::Options opts(get_app_name());

    opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    opts.add_option("",
                    "p",
                    "partitioner",
                    "Name of the partitioner",
                    cxxopts::value<std::string>(),
                    "");
    opts.add_option("", "", "mesh-file", "The mesh file name", cxxopts::value<std::string>(), "");
    opts.positional_help("<mesh-file>");
    return opts;
}

cxxopts::ParseResult
MeshPartApp::parse(cxxopts::Options & opts)
{
    opts.parse_positional({ "mesh-file" });
    return CommandLineInterface::parse(opts);
}

int
MeshPartApp::run()
{
    if (this->cli_result.count("help"))
        fmt::print("{}", this->cli_opts.help());
    else if (this->cli_result.count("version"))
        fmt::print("{}, version {}\n", get_name(), get_version());
    else if (this->cli_result.count("mesh-file"))
        partition_mesh_file(this->cli_result["mesh-file"].as<std::string>());
    else
        fmt::print("{}", this->cli_opts.help());
    return 0;
}

Qtr<UnstructuredMesh>
MeshPartApp::load_mesh(const std::string & file_name)
{
    auto pars = FileMesh::parameters();
    pars.set<App *>("app", this);
    pars.set<fs::path>("file", file_name);
    return MeshFactory::create<FileMesh>(pars);
}

void
MeshPartApp::partition_mesh_file(const std::string & mesh_file_name)
{
    auto mesh = load_mesh(mesh_file_name);
    mesh->distribute(0);

    auto out_file_name = fmt::format("out");
    save_partition(mesh.get(), out_file_name);
}

void
MeshPartApp::save_partition(UnstructuredMesh * mesh, const std::string & file_name)
{
    auto pars = ExodusIIOutput::parameters();
    pars.set<App *>("app", this);
    pars.set<fs::path>("file", file_name);
    ExodusIIOutput out(pars);
    // FIXME: this needs exodusII refactoring done
    // out.create();
    // out.output_mesh();
}

// ---

int
main(int argc, char * argv[])
{
    Init init(argc, argv);
    MeshPartApp app(argc, argv);
    try {
        app.run();
        return 0;
    }
    catch (Exception & e) {
        fmt::println("{}", e.what());
        return -1;
    }
}
