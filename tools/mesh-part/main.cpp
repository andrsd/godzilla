#include "cxxopts/cxxopts.hpp"
#include "fmt/printf.h"
#include "godzilla/Init.h"
#include "godzilla/App.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/ExodusIIOutput.h"

const char * version = "1.0";

using namespace godzilla;

class MeshPartApp : public App {
public:
    MeshPartApp(int argc, const char * const * argv);

    void process_command_line(cxxopts::ParseResult & result) override;

protected:
    void create_command_line_options() override;
    const std::string & get_input_file_name() const override;
    UnstructuredMesh * load_mesh(const std::string & file_name);
    void partition_mesh_file(const std::string & mesh_file_name);
    void save_partition(UnstructuredMesh * mesh, const std::string & file_name);

private:
    std::string input_file_name;
};

MeshPartApp::MeshPartApp(int argc, const char * const * argv) :
    App(mpi::Communicator(PETSC_COMM_WORLD), "mesh-part", argc, argv),
    input_file_name(".")
{
}

const std::string &
MeshPartApp::get_input_file_name() const
{
    return this->input_file_name;
}

void
MeshPartApp::process_command_line(cxxopts::ParseResult & result)
{
    this->cmdln_opts.parse_positional({ "mesh-file" });
    auto res = this->cmdln_opts.parse(argc, argv);
    if (res.count("help"))
        fmt::print("{}", this->cmdln_opts.help());
    else if (res.count("version"))
        fmt::print("{}, version {}\n", name(), version());
    else if (res.count("mesh-file"))
        partition_mesh_file(res["mesh-file"].as<std::string>());
    else
        fmt::print("{}", this->cmdln_opts.help());
}

void
MeshPartApp::create_command_line_options()
{
    this->cmdln_opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    this->cmdln_opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    this->cmdln_opts.add_option("",
                                "p",
                                "partitioner",
                                "Name of the partitioner",
                                cxxopts::value<std::string>(),
                                "");
    this->cmdln_opts
        .add_option("", "", "mesh-file", "The mesh file name", cxxopts::value<std::string>(), "");
    this->cmdln_opts.positional_help("<mesh-file>");
}

UnstructuredMesh *
MeshPartApp::load_mesh(const std::string & file_name)
{
    auto pars = ExodusIIMesh::parameters();
    pars.set<App *>("_app") = this;
    pars.set<std::string>("file") = file_name;
    UnstructuredMesh * mesh = new ExodusIIMesh(pars);
    mesh->create();
    return mesh;
}

void
MeshPartApp::partition_mesh_file(const std::string & mesh_file_name)
{
    auto mesh = load_mesh(mesh_file_name);
    mesh->distribute();

    std::string out_file_name = fmt::format("out");
    save_partition(mesh, out_file_name);
}

void
MeshPartApp::save_partition(UnstructuredMesh * mesh, const std::string & file_name)
{
    auto pars = ExodusIIOutput::parameters();
    pars.set<App *>("_app") = this;
    pars.set<UnstructuredMesh *>("_mesh") = mesh;
    pars.set<std::string>("file") = file_name;
    ExodusIIOutput out(pars);
    out.create();
    out.output_mesh();
}

// ---

int
main(int argc, char * argv[])
{
    Init init(argc, argv);
    MeshPartApp app(argc, argv);
    app.run();
    return 0;
}
