#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "TestApp.h"
#include "godzilla/FileMesh.h"
#include "godzilla/MeshFactory.h"
#include <filesystem>

using namespace godzilla;
namespace fs = std::filesystem;

TEST(FileMesh, exoii_file_format)
{
    TestApp app;

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "mesh" / "2blk.exo";

    auto mesh_pars = app.make_parameters<FileMesh>();
    mesh_pars.set<fs::path>("file", file);
    FileMesh mesh(mesh_pars);
    mesh.create();

    EXPECT_TRUE(mesh.get_file_format() == FileMesh::EXODUSII);
}

TEST(FileMesh, non_existing_file_stops_execution)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<FileMesh>();
    mesh_pars.set<fs::path>("file", "non-existing-file");
    EXPECT_DEATH(MeshFactory::create<FileMesh>(mesh_pars),
                 "Unable to open 'non-existing-file' for reading. Make sure it exists and you have "
                 "read permissions.");
}

TEST(FileMesh, unknown_mesh_format)
{
    TestApp app;

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "yml" / "empty.yml";

    auto mesh_pars = app.make_parameters<FileMesh>();
    mesh_pars.set<fs::path>("file", file);
    EXPECT_DEATH(MeshFactory::create<FileMesh>(mesh_pars), "Unknown mesh format");
}

TEST(FileMesh, gmsh_distributed_error)
{
    TestApp app;

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "mesh" / "quad.msh";

    auto mesh_pars = app.make_parameters<FileMesh>();
    mesh_pars.set<fs::path>("file", file);
    mesh_pars.set<bool>("distributed", true);
    EXPECT_DEATH(MeshFactory::create<FileMesh>(mesh_pars),
                 "Distributed mesh is not supported for GMSH.");
}

TEST(FileMesh, distributed_mesh)
{
    TestApp app;
    auto comm = app.get_comm();

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "mesh" / "dist-square-tris.exo";

    auto mesh_pars = app.make_parameters<FileMesh>();
    mesh_pars.set<fs::path>("file", file);
    mesh_pars.set<bool>("distributed", true);
    auto mesh = MeshFactory::create<FileMesh>(mesh_pars);
    EXPECT_NE(mesh, nullptr);
    if (comm.size() > 1) {
        EXPECT_TRUE(mesh->is_distributed());
    }
}
