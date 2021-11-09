#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "yaml-cpp/yaml.h"
#include "GodzillaConfig.h"
#include "G1DLineMesh.h"


using namespace godzilla;

registerObject(GTestProblem);

InputParameters
GTestProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addParam<std::string>("str", "empty", "str doco");
    params.addParam<double>("d", 1.234, "d doco");
    params.addParam<int>("i", -1234, "i doco");
    params.addParam<unsigned int>("ui", 1234, "ui doco");
    params.addParam<std::vector<double>>("arr_d", "vec<d> doco");
    params.addPrivateParam<Grid *>("_ggrid");
    return params;
}


TEST_F(GYMLFileTest, parse_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    auto yml = file.getYml();
    EXPECT_EQ(yml.size(), 0);

}

TEST_F(GYMLFileTest, build_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    EXPECT_DEATH(
        file.build(),
        "ERROR: Missing 'grid' block."
    );
}

TEST_F(GYMLFileTest, build_grid_no_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/grid_no_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(
        file.build(),
        "ERROR: grid: No 'type' specified."
    );
}

TEST_F(GYMLFileTest, build_grid_unreg_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/grid_unreg_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(
        file.build(),
        "ERROR: grid: Type 'ASDF' is not a registered object."
    );
}

TEST_F(GYMLFileTest, build_missing_req_param)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/grid_missing_req_param.yml");

    file.parse(file_name);
    EXPECT_DEATH(
        file.build(),
        "ERROR: grid: Missing required parameters:"
    );
}

TEST_F(GYMLFileTest, build)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/simple.yml");

    file.parse(file_name);
    file.build();

    auto grid = dynamic_cast<G1DLineMesh *>(file.getGrid());
    EXPECT_NE(grid, nullptr);

    auto problem = file.getProblem();
    EXPECT_NE(problem, nullptr);

    auto exec = file.getExecutioner();
}
