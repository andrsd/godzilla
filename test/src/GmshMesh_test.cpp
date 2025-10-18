#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/GmshMesh.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

#if 0
TEST(GmshMeshTest, api)
{
    TestApp app;
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/quad.msh");

    auto params = GmshMesh::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "obj");
    params.set<std::string>("file", file_name);
    GmshMesh mesh(params);

    EXPECT_EQ(mesh.get_file_name(), file_name);

    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto dm = m->get_dm();

    EXPECT_EQ(m->get_dimension(), 2);

    Real gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 0);
    EXPECT_EQ(gmax[0], 1);

    EXPECT_EQ(gmin[1], 0);
    EXPECT_EQ(gmax[1], 1);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    Int n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 10);
}

TEST(GmshMeshTest, nonexitent_file)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto params = GmshMesh::parameters();
    params.set<App *>("_app", &app);
    params.set<std::string>("_name", "obj");
    params.set<std::string>("file", "asdf.msh");
    GmshMesh mesh(params);

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "obj: Unable to open 'asdf.msh' for reading. Make sure it exists and you "
                    "have read permissions."));
}
#endif
