#include "GodzillaConfig.h"
#include "base/GodzillaApp_test.h"
#include "grids/GExodusIIMesh_test.h"
#include "utils/InputParameters.h"
#include "petsc.h"


using namespace godzilla;

registerObject(MockGExodusIIMesh);

TEST_F(GExodusIIMeshTest, gexodus_mesh)
{
    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/square.e");
    auto obj = gExodusMesh(file_name);

    EXPECT_EQ(obj->getFileName(), file_name);

    delete obj;
}

TEST_F(GExodusIIMeshTest, gexodus_mesh_nonexitent_file)
{
    EXPECT_DEATH(
        gExodusMesh("asdf.e"),
        "ERROR: obj: Unable to open 'asdf.e' for reading. Make sure it exists and you have read permissions."
    );
}

TEST_F(GExodusIIMeshTest, gexodus_mesh_create)
{
    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/square.e");
    auto obj = gExodusMesh(file_name);
    obj->create();
    const DM & dm = obj->getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);
    EXPECT_EQ(dim, 2);

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

    delete obj;
}
