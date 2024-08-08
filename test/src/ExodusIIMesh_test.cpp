#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/Parameters.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;
using testing::ElementsAre;
using testing::Pair;

TEST(ExodusIIMeshTest, api)
{
    TestApp app;
    std::string file_name = std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/square.e");

    Parameters params = ExodusIIMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);

    EXPECT_EQ(mesh.get_file_name(), file_name);

    mesh.create();
    auto m = mesh.get_mesh<Mesh>();
    auto dm = m->get_dm();

    EXPECT_EQ(m->get_dimension(), 2);

    Real gmin[4], gmax[4];
    DMGetBoundingBox(dm, gmin, gmax);
    EXPECT_EQ(gmin[0], 0);
    EXPECT_EQ(gmax[0], 1);

    EXPECT_EQ(gmin[1], 0);
    EXPECT_EQ(gmax[1], 1);

    Vec coords;
    PETSC_CHECK(DMGetCoordinates(dm, &coords));
    Int n;
    PETSC_CHECK(VecGetSize(coords, &n));
    EXPECT_EQ(n, 18);
}

TEST(ExodusIIMeshTest, two_block)
{
    TestApp app;
    std::string file_name = std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/2blk.exo");
    Parameters params = ExodusIIMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    EXPECT_EQ(m->get_cell_set_name(0), "0");
    EXPECT_EQ(m->get_cell_set_name(1), "1");

    EXPECT_EQ(m->get_cell_set_id("0"), 0);
    EXPECT_EQ(m->get_cell_set_id("1"), 1);

    auto cell_sets = m->get_cell_sets();
    EXPECT_THAT(cell_sets, ElementsAre(Pair(0, "0"), Pair(1, "1")));
}

TEST(ExodusIIMeshTest, two_block_nonexistent_blk)
{
    TestApp app;
    std::string file_name = std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/2blk.exo");
    Parameters params = ExodusIIMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    EXPECT_THROW_MSG({ auto & name = m->get_cell_set_name(1234); },
                     "Cell set ID '1234' does not exist.");
    EXPECT_THROW_MSG({ auto id = m->get_cell_set_id("1234"); }, "Cell set '1234' does not exist.");
}

TEST(ExodusIIMeshTest, nonexitent_file)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters params = ExodusIIMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("file") = "asdf.e";
    ExodusIIMesh mesh(params);

    app.check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("obj: Unable to open 'asdf.e' for reading. Make sure it exists and you "
                           "have read permissions."));
}
