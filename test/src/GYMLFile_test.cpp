#include "GodzillaApp_test.h"
#include "GYMLFile_test.h"
#include "yaml-cpp/yaml.h"
#include "Common.h"
#include "Problem.h"

using namespace godzilla;

class GTestProblem : public Problem {
public:
    GTestProblem(const InputParameters & params) : Problem(params)
    {
        DMPlexCreateBoxMesh(comm(), 1, PETSC_TRUE, NULL, NULL, NULL, NULL, PETSC_FALSE, &this->dm);
        DMSetUp(this->dm);
        DMCreateGlobalVector(this->dm, &this->x);
    }

    virtual ~GTestProblem()
    {
        VecDestroy(&this->x);
        DMDestroy(&this->dm);
    }

    DM
    get_dm() const override
    {
        return this->dm;
    }
    Vec
    get_solution_vector() const override
    {
        return this->x;
    }
    void
    create() override
    {
    }
    void
    solve() override
    {
    }
    void
    run() override
    {
    }
    bool
    converged() override
    {
        return false;
    }

protected:
    DM dm;
    Vec x;
public:
    static InputParameters valid_params();
};
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
    return params;
}

TEST_F(GYMLFileTest, parse_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    auto yml = file.get_yml();
    EXPECT_EQ(yml.size(), 0);
}

TEST_F(GYMLFileTest, build_empty)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/empty.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: Missing 'mesh' block.");
}

TEST_F(GYMLFileTest, build_no_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/no_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: mesh: No 'type' specified.");
}

TEST_F(GYMLFileTest, build_unreg_type)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/unreg_type.yml");

    file.parse(file_name);
    EXPECT_DEATH(file.build(), "error: mesh: Type 'ASDF' is not a registered object.");
}

TEST_F(GYMLFileTest, objects)
{
    class MockGYMLFile : public GYMLFile {
    public:
        MockGYMLFile(const App & app) : GYMLFile(app) {}
        void
        add_object(Object * obj)
        {
            GYMLFile::add_object(obj);
        }
        void
        check_params(const InputParameters & params, const std::string & name)
        {
            GYMLFile::check_params(params, name);
        }
    };

    class MockObject : public Object {
    public:
        MockObject(const InputParameters & params) : Object(params) {}
        MOCK_METHOD(void, create, (), (override));
        MOCK_METHOD(void, check, (), (override));
    };

    InputParameters obj_pars = Object::valid_params();
    obj_pars.set<const App *>("_app") = this->app;
    obj_pars.set<std::string>("_name") = "obj";
    MockObject obj(obj_pars);

    MockGYMLFile file(*this->app);
    file.check_params(obj_pars, "obj");
    file.add_object(&obj);
    EXPECT_CALL(obj, create()).Times(1);
    file.create();
    EXPECT_CALL(obj, check()).Times(1);
    file.check();
}

TEST_F(GYMLFileTest, build_missing_req_param)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/mesh_missing_req_param.yml");
    file.parse(file_name);
    file.build();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("mesh: Missing required parameters:"));
}

TEST_F(GYMLFileTest, param_types)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/param_types.yml");

    file.parse(file_name);
    file.build();

    EXPECT_NE(file.get_problem(), nullptr);
}

TEST_F(GYMLFileTest, build_vec_as_scalars)
{
    GYMLFile file(*this->app);

    std::string file_name =
        std::string(GODZILLA_UNIT_TESTS_ROOT) + std::string("/assets/vec_as_scalars.yml");

    file.parse(file_name);
    file.build();

    EXPECT_NE(file.get_problem(), nullptr);
}

TEST_F(GYMLFileTest, wrong_param_type)
{
    testing::internal::CaptureStderr();

    GYMLFile file(*this->app);
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
