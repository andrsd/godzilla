// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "cxxopts/cxxopts.hpp"
#include "fmt/printf.h"
#include "godzilla/Init.h"
#include "godzilla/App.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/ExodusIIOutput.h"

const char * version = "1.0";

#if 0
using namespace godzilla;

class MeshPartApp : public App {
public:
    MeshPartApp(int argc, const char * const * argv);

    void process_command_line(const cxxopts::ParseResult & result) override;

protected:
    void create_command_line_options() override;
    MeshObject * load_mesh(const std::string & file_name);
    void partition_mesh_file(const std::string & mesh_file_name);
    void save_partition(UnstructuredMesh * mesh, const std::string & file_name);
};

MeshPartApp::MeshPartApp(int argc, const char * const * argv) :
    App(mpi::Communicator(PETSC_COMM_WORLD), "mesh-part", argc, argv)
{
}

void
MeshPartApp::process_command_line(const cxxopts::ParseResult & result)
{
    get_command_line_opts().parse_positional({ "mesh-file" });
    auto res = parse_command_line();
    if (res.count("help"))
        fmt::print("{}", get_command_line_opts().help());
    else if (res.count("version"))
        fmt::print("{}, version {}\n", get_name(), get_version());
    else if (res.count("mesh-file"))
        partition_mesh_file(res["mesh-file"].as<std::string>());
    else
        fmt::print("{}", get_command_line_opts().help());
}

void
MeshPartApp::create_command_line_options()
{
    auto opts = get_command_line_opts();
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
}

MeshObject *
MeshPartApp::load_mesh(const std::string & file_name)
{
    auto pars = ExodusIIMesh::parameters();
    pars.set<App *>("_app", this);
    pars.set<std::string>("file", file_name);
    auto mesh = new ExodusIIMesh(pars);
    mesh->create();
    return mesh;
}

void
MeshPartApp::partition_mesh_file(const std::string & mesh_file_name)
{
    auto mesh_obj = load_mesh(mesh_file_name);
    auto mesh = mesh_obj->get_mesh<UnstructuredMesh>();
    mesh->distribute(0);

    std::string out_file_name = fmt::format("out");
    save_partition(mesh, out_file_name);
}

void
MeshPartApp::save_partition(UnstructuredMesh * mesh, const std::string & file_name)
{
    auto pars = ExodusIIOutput::parameters();
    pars.set<App *>("_app", this);
    pars.set<UnstructuredMesh *>("_mesh", mesh);
    pars.set<std::string>("file", file_name);
    ExodusIIOutput out(pars);
    out.create();
    out.output_mesh();
}
#endif
// ---

int
main(int argc, char * argv[])
{
#if 0
    Init init(argc, argv);
    MeshPartApp app(argc, argv);
    app.run();
#endif
    return 0;
}
