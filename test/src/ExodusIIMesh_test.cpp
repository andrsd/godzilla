#include "GodzillaApp_test.h"
#include "Common.h"
#include "Error.h"
#include "CallStack.h"
#include "Mesh.h"
#include "ExodusIIMesh.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(ExodusIIMeshTest, load_hex)
{
    TestApp app;
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/box-2x2x2.e");

    InputParameters params = ExodusIIMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);

    EXPECT_EQ(mesh.get_file_name(), file_name);

    mesh.create();
    EXPECT_EQ(mesh.get_num_elements(), 8);

    EXPECT_EQ(mesh.get_dimension(), 3);
}

TEST(ExodusIIMeshTest, load_tri)
{
    TestApp app;
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/square-tri.e");

    InputParameters params = ExodusIIMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("file") = file_name;
    ExodusIIMesh mesh(params);

    EXPECT_EQ(mesh.get_file_name(), file_name);

    mesh.create();
    EXPECT_EQ(mesh.get_num_elements(), 4);

    EXPECT_EQ(mesh.get_dimension(), 2);
}
