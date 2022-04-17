#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "yaml-cpp/yaml.h"
#include "GodzillaConfig.h"

using namespace godzilla;

TEST_F(GYMLFileTest, parse_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    auto yml = file.getYml();
    EXPECT_EQ(yml.size(), 0);
}
