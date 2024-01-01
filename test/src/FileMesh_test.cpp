#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/FileMesh.h"

using namespace godzilla;
using namespace testing;

TEST(FileMesh, check)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = FileMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    mesh_pars.set<std::string>("file") =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");
    FileMesh mesh(mesh_pars);
    mesh.check();

    ASSERT_FALSE(app.check_integrity());
    auto out = testing::internal::GetCapturedStderr();
    EXPECT_THAT(out, HasSubstr("[ERROR] Unknown mesh format"));
}
