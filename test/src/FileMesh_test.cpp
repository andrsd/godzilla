#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "TestApp.h"
#include "godzilla/FileMesh.h"
#include "godzilla/MeshFactory.h"

using namespace godzilla;
using namespace testing;

TEST(FileMesh, exoii_file_format)
{
    TestApp app;

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    mesh_pars.set<String>("file",
                          String(GODZILLA_UNIT_TESTS_ROOT) + String("/assets/mesh/2blk.exo"));
    FileMesh mesh(mesh_pars);
    mesh.create();

    EXPECT_TRUE(mesh.get_file_format() == FileMesh::EXODUSII);
}

TEST(FileMesh, unknown_mesh_format)
{
    TestApp app;

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("app", &app);
    mesh_pars.set<String>("file",
                          String(GODZILLA_UNIT_TESTS_ROOT) + String("/assets/yml/empty.yml"));
    EXPECT_THROW_MSG(MeshFactory::create<FileMesh>(mesh_pars), "Unknown mesh format");
}
