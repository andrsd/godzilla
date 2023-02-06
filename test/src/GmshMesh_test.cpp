#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "GmshMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(GmshMeshTest, api)
{
    TestApp app;
    std::string file_name = std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/quad.msh");

    Parameters params = GmshMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = file_name;
    GmshMesh mesh(params);

    EXPECT_EQ(mesh.get_file_name(), file_name);

    mesh.create();
    DM dm = mesh.get_dm();

    EXPECT_EQ(mesh.get_dimension(), 2);

    PetscReal gmin[4], gmax[4];
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

    Parameters params = GmshMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = "asdf.msh";
    GmshMesh mesh(params);

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "obj: Unable to open 'asdf.msh' for reading. Make sure it exists and you "
                    "have read permissions."));
}
