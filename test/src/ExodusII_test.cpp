#include "GodzillaConfig.h"
#include "Error.h"
#include "CallStack.h"
#include "Mesh.h"
#include "ExodusIO.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(ExodusIOTest, load_hex)
{
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/box-2x2x2.e");

    Mesh mesh = ExodusIO::load(file_name);
    EXPECT_EQ(mesh.get_num_elements(), 8);
}

TEST(ExodusIOTest, load_tri)
{
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/square-tri.e");

    Mesh mesh = ExodusIO::load(file_name);
    EXPECT_EQ(mesh.get_num_elements(), 4);
}

TEST(ExodusIOTest, load_edge)
{
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh/line-2.e");

    Mesh mesh = ExodusIO::load(file_name);
    EXPECT_EQ(mesh.get_num_elements(), 2);
}
