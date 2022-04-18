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
    EXPECT_EQ(mesh.getNumElements(), 8);
}
