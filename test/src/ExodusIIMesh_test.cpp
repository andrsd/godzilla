#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "ExodusIIMesh.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(ExodusIIMeshTest, api)
{
    TestApp app;
    std::string file_name = std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/square.e");

    InputParameters params = ExodusIIMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);

    EXPECT_EQ(mesh.getFileName(), file_name);

    mesh.create();
    DM dm = mesh.getDM();

    EXPECT_EQ(mesh.getDimension(), 2);

    PetscReal gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 0);
    EXPECT_EQ(gmax[0], 1);

    EXPECT_EQ(gmin[1], 0);
    EXPECT_EQ(gmax[1], 1);

    Vec coords;
    DMGetCoordinates(dm, &coords);
    PetscInt n;
    VecGetSize(coords, &n);
    EXPECT_EQ(n, 18);
}

TEST(ExodusIIMeshTest, nonexitent_file)
{
    testing::internal::CaptureStderr();

    TestApp app;

    InputParameters params = ExodusIIMesh::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = "asdf.e";
    ExodusIIMesh mesh(params);

    app.checkIntegrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("obj: Unable to open 'asdf.e' for reading. Make sure it exists and you "
                           "have read permissions."));
}
