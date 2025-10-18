#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/FileMesh.h"
#include "godzilla/MeshFactory.h"

using namespace godzilla;
using namespace testing;

TEST(FileMesh, exoii_file_format)
{
    TestApp app;

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app", &app);
    mesh_pars.set<std::string>("file",
                               std::string(GODZILLA_UNIT_TESTS_ROOT) +
                                   std::string("/assets/mesh/2blk.exo"));
    FileMesh mesh(mesh_pars);
    mesh.create();

    EXPECT_TRUE(mesh.get_file_format() == FileMesh::EXODUSII);
}

TEST(FileMesh, unknown_mesh_format)
{
    TestApp app;

    auto mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app", &app);
    mesh_pars.set<std::string>("file",
                               std::string(GODZILLA_UNIT_TESTS_ROOT) +
                                   std::string("/assets/yml/empty.yml"));
    auto mesh = MeshFactory::create<FileMesh>(mesh_pars);

    EXPECT_FALSE(app.check_integrity());

    testing::internal::CaptureStderr();
    app.get_logger()->print();
    auto out = testing::internal::GetCapturedStderr();
    EXPECT_THAT(out, HasSubstr("[ERROR] Unknown mesh format"));
}
