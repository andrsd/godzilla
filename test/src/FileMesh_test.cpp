#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "TestApp.h"
#include "godzilla/FileMesh.h"
#include "godzilla/MeshFactory.h"
#include <filesystem>

using namespace godzilla;
using namespace testing;
namespace fs = std::filesystem;

TEST(FileMesh, exoii_file_format)
{
    TestApp app;

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "mesh" / "2blk.exo";

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    mesh_pars.set<fs::path>("file", file);
    FileMesh mesh(mesh_pars);
    mesh.create();

    EXPECT_TRUE(mesh.get_file_format() == FileMesh::EXODUSII);
}

TEST(FileMesh, non_existing_file_stops_execution)
{
    TestApp app;

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    mesh_pars.set<fs::path>("file", "non-existing-file");
    EXPECT_DEATH(MeshFactory::create<FileMesh>(mesh_pars),
                 "Unable to open 'non-existing-file' for reading. Make sure it exists and you have "
                 "read permissions.");
}

TEST(FileMesh, unknown_mesh_format)
{
    TestApp app;

    auto file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "yml" / "empty.yml";

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    mesh_pars.set<fs::path>("file", file);
    EXPECT_DEATH(MeshFactory::create<FileMesh>(mesh_pars), "Unknown mesh format");
}
