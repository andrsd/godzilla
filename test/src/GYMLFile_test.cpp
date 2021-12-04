#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "yaml-cpp/yaml.h"
#include "GodzillaConfig.h"
#include "LineMesh.h"
#include "PiecewiseLinear.h"
#include "FEProblemInterface.h"

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
    params.addParam<std::vector<int>>("arr_i", "vec<i> doco");
    params.addParam<std::vector<std::string>>("arr_str", "vec<str> doco");
    params.addPrivateParam<Grid *>("_grid");
    return params;
}

TEST_F(GYMLFileTest, parse_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    auto yml = file.getYml();
    EXPECT_EQ(yml.size(), 0);
}

TEST_F(GYMLFileTest, build_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: Missing 'grid' block.");
}

TEST_F(GYMLFileTest, build_grid_no_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/grid_no_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: grid: No 'type' specified.");
}

TEST_F(GYMLFileTest, build_grid_unreg_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/grid_unreg_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: grid: Type 'ASDF' is not a registered object.");
}

TEST_F(GYMLFileTest, build_missing_req_param)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/grid_missing_req_param.yml");
    file.parse(file_name);
    file.build();
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("grid: Missing required parameters:"));
}

TEST_F(GYMLFileTest, build)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple.yml");

    file.parse(file_name);
    file.build();

    auto grid = dynamic_cast<LineMesh *>(file.getGrid());
    EXPECT_NE(grid, nullptr);

    auto problem = file.getProblem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, funcs)
{
    class TestGYMLFile : public GYMLFile {
    public:
        TestGYMLFile(const App & app) : GYMLFile(app) {}
        void
        buildFunctions()
        {
            GYMLFile::buildFunctions();
        }
    } file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/funcs.yml");

    file.parse(file_name);
    file.buildFunctions();

    const std::vector<Function *> & funcs = file.getFunctions();
    EXPECT_EQ(funcs.size(), 1);
    EXPECT_NE(dynamic_cast<PiecewiseLinear *>(funcs[0]), nullptr);
}

TEST_F(GYMLFileTest, build_vec_as_scalars)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_vec_as_scalars.yml");

    file.parse(file_name);
    file.build();

    auto grid = dynamic_cast<LineMesh *>(file.getGrid());
    EXPECT_NE(grid, nullptr);

    auto problem = file.getProblem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, wrong_param_type)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/wrong_param_type.yml");
    file.parse(file_name);
    file.build();
    this->app->checkIntegrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output,
                testing::HasSubstr(
                    "Parameter 'arr_d' must be either a single value or a vector of values."));
}

TEST_F(GYMLFileTest, build_fe)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_fe.yml");

    file.parse(file_name);
    file.build();

    auto grid = dynamic_cast<LineMesh *>(file.getGrid());
    EXPECT_NE(grid, nullptr);

    auto problem = file.getProblem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, build_fe_w_aux)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_fe_w_aux.yml");

    file.parse(file_name);
    file.build();

    auto grid = dynamic_cast<LineMesh *>(file.getGrid());
    EXPECT_NE(grid, nullptr);

    auto problem = dynamic_cast<FEProblemInterface *>(file.getProblem());
    EXPECT_NE(problem, nullptr);
    // TODO: test that the 'aux1' object from the input file was actually added
}

TEST_F(GYMLFileTest, nonfe_problem_with_ics)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_ics.yml");
    file.parse(file_name);
    file.build();
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support initial conditions."));
}

TEST_F(GYMLFileTest, nonfe_problem_with_bcs)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_bcs.yml");
    file.parse(file_name);
    file.build();
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support boundary conditions."));
}

TEST_F(GYMLFileTest, nonfe_problem_with_auxs)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_auxs.yml");
    file.parse(file_name);
    file.build();
    this->app->checkIntegrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support auxiliary fields."));
}
