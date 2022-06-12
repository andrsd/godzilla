#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "yaml-cpp/yaml.h"
#include "GodzillaConfig.h"
#include "LineMesh.h"
#include "PiecewiseLinear.h"
#include "Postprocessor.h"
#include "FEProblemInterface.h"

using namespace godzilla;

registerObject(GTestProblem);

InputParameters
GTestProblem::valid_params()
{
    InputParameters params = Problem::valid_params();
    params.add_param<std::string>("str", "empty", "str doco");
    params.add_param<double>("d", 1.234, "d doco");
    params.add_param<int>("i", -1234, "i doco");
    params.add_param<unsigned int>("ui", 1234, "ui doco");
    params.add_param<std::vector<double>>("arr_d", "vec<d> doco");
    params.add_param<std::vector<int>>("arr_i", "vec<i> doco");
    params.add_param<std::vector<std::string>>("arr_str", "vec<str> doco");
    params.add_param<std::map<std::string, PetscReal>>("consts", "map<str, real> doco");
    params.add_param<std::map<std::string, std::vector<std::string>>>("fns",
                                                                      "map<str, vec<str>> doco");
    params.add_private_param<const Mesh *>("_mesh");
    return params;
}

TEST_F(GYMLFileTest, parse_empty)
{
    class TestGYMLFile : public GYMLFile {
    public:
        explicit TestGYMLFile(const App * app) : GYMLFile(app) {}

        const YAML::Node &
        get_yml()
        {
            return this->root;
        }
    };

    TestGYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    auto yml = file.get_yml();
    EXPECT_EQ(yml.size(), 0);
}

TEST_F(GYMLFileTest, build_empty)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "\\[ERROR\\] Missing 'mesh' block.");
}

TEST_F(GYMLFileTest, build_mesh_no_type)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh_no_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "\\[ERROR\\] mesh: No 'type' specified.");
}

TEST_F(GYMLFileTest, build_mesh_unreg_type)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh_unreg_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "\\[ERROR\\] mesh: Type 'ASDF' is not a registered object.");
}

TEST_F(GYMLFileTest, build_missing_req_param)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh_missing_req_param.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("mesh: Missing required parameters:"));
}

TEST_F(GYMLFileTest, mesh_partitioner)
{
    PetscMPIInt sz;
    MPI_Comm_size(PETSC_COMM_WORLD, &sz);
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh_partitioner.yml");

    file.parse(file_name);
    file.build();

    Mesh * mesh = file.get_mesh();
    EXPECT_NE(mesh, nullptr);
    mesh->create();

    DM dm = mesh->get_dm();
    PetscBool distr;
    DMPlexIsDistributed(dm, &distr);
    if (sz > 1)
        EXPECT_EQ(distr, 1);
    else
        EXPECT_EQ(distr, 0);
}

TEST_F(GYMLFileTest, build)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple.yml");

    EXPECT_TRUE(file.parse(file_name));
    file.build();

    auto mesh = dynamic_cast<LineMesh *>(file.get_mesh());
    EXPECT_NE(mesh, nullptr);

    auto problem = file.get_problem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, funcs)
{
    class TestGYMLFile : public GYMLFile {
    public:
        explicit TestGYMLFile(const App * app) : GYMLFile(app) {}
        void
        build()
        {
            GYMLFile::build_problem();
            GYMLFile::build_functions();
        }
    } file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/funcs.yml");

    file.parse(file_name);
    file.build();

    auto * problem = file.get_problem();
    const std::vector<Function *> & funcs = problem->get_functions();
    EXPECT_EQ(funcs.size(), 1);
    EXPECT_NE(dynamic_cast<PiecewiseLinear *>(funcs[0]), nullptr);
}

TEST_F(GYMLFileTest, build_vec_as_scalars)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_vec_as_scalars.yml");

    file.parse(file_name);
    file.build();

    auto mesh = dynamic_cast<LineMesh *>(file.get_mesh());
    EXPECT_NE(mesh, nullptr);

    auto problem = file.get_problem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, wrong_param_type)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/wrong_param_type.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    auto output = testing::internal::GetCapturedStderr();
    EXPECT_THAT(output,
                testing::HasSubstr(
                    "Parameter 'arr_d' must be either a single value or a vector of values."));
}

TEST_F(GYMLFileTest, build_fe)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_fe.yml");

    file.parse(file_name);
    file.build();

    auto mesh = dynamic_cast<LineMesh *>(file.get_mesh());
    EXPECT_NE(mesh, nullptr);

    auto problem = file.get_problem();
    EXPECT_NE(problem, nullptr);
}

TEST_F(GYMLFileTest, simple_fe_pps)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_fe_pps.yml");

    file.parse(file_name);
    file.build();

    auto mesh = dynamic_cast<LineMesh *>(file.get_mesh());
    EXPECT_NE(mesh, nullptr);

    auto problem = file.get_problem();
    EXPECT_NE(problem, nullptr);

    Postprocessor * pp = problem->get_postprocessor("l2_err");
    EXPECT_NE(pp, nullptr);
    EXPECT_EQ(pp->get_name(), "l2_err");
}

TEST_F(GYMLFileTest, build_fe_w_aux)
{
    GYMLFile file(this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/simple_fe_w_aux.yml");

    file.parse(file_name);
    file.build();

    auto mesh = dynamic_cast<LineMesh *>(file.get_mesh());
    EXPECT_NE(mesh, nullptr);

    auto problem = dynamic_cast<FEProblemInterface *>(file.get_problem());
    EXPECT_NE(problem, nullptr);
    // TODO: test that the 'aux1' object from the input file was actually added
}

TEST_F(GYMLFileTest, nonfe_problem_with_ics)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_ics.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support initial conditions."));
}

TEST_F(GYMLFileTest, nonfe_problem_with_bcs)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_bcs.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support boundary conditions."));
}

TEST_F(GYMLFileTest, nonfe_problem_with_auxs)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/nonfe_with_auxs.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Supplied problem type 'GTestProblem' does not support auxiliary fields."));
}

TEST_F(GYMLFileTest, unused_param)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/unused_param.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("mesh: Following parameters were not used: ny"));
}

TEST_F(GYMLFileTest, malformed)
{
    testing::internal::CaptureStderr();

    GYMLFile file(this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/malformed.yml");
    EXPECT_FALSE(file.parse(file_name));
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Failed to parse the input file: error at line 9, column 7: "
                                   "end of map flow not found"));
}
