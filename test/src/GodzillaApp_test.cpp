#include "GodzillaConfig.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "Mesh.h"
#include "Problem.h"
#include "GYMLFile.h"
#include "petsc.h"

using namespace godzilla;

namespace {

class MockMesh : public Mesh {
public:
    explicit MockMesh(const Parameters & params) : Mesh(params) {}

    virtual DM
    get_dm() const
    {
        return nullptr;
    }

protected:
    virtual void
    distribute()
    {
    }
};

class MockProblem : public Problem {
public:
    explicit MockProblem(const Parameters & params) : Problem(params) {}

    MOCK_METHOD(void, create, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, solve, ());
    MOCK_METHOD(bool, converged, ());
    MOCK_METHOD(DM, get_dm, (), (const));
    MOCK_METHOD(Vec, get_solution_vector, (), (const));
};

} // namespace

REGISTER_OBJECT(MockMesh);
REGISTER_OBJECT(MockProblem);

TEST_F(GodzillaAppTest, run_input)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla",
                      (char *) "-i",
                      (char *) GODZILLA_UNIT_TESTS_ROOT "/assets/simple.yml",
                      NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.parse_command_line(argc, argv);
    app.run();

    EXPECT_EQ(app.get_input_file_name(), GODZILLA_UNIT_TESTS_ROOT "/assets/simple.yml");

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST_F(GodzillaAppTest, run_input_non_existent_file)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla",
                      (char *) "-i",
                      (char *) GODZILLA_UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
                      NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.parse_command_line(argc, argv);

    EXPECT_DEATH(app.run(), "\\[ERROR\\] Unable to open");
}

TEST_F(GodzillaAppTest, no_colors)
{
    int argc = 2;
    char * argv[] = { (char *) "godzilla", (char *) "--no-colors", NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.parse_command_line(argc, argv);

    app.run();
    EXPECT_EQ(Terminal::num_colors, 1);
}

TEST_F(GodzillaAppTest, verbose)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla", (char *) "--verbose", (char *) "2", NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.parse_command_line(argc, argv);

    app.run();
    EXPECT_EQ(app.get_verbosity_level(), 2);
}

TEST_F(GodzillaAppTest, check_integrity)
{
    class TestApp : public App {
    public:
        TestApp() : App("godzilla", MPI_COMM_WORLD) {}

        void
        run()
        {
            this->gyml = allocate_gyml();
            this->log->error("error1");
            check_integrity();
        }
    } app;

    EXPECT_DEATH(app.run(), "error1");
}

TEST_F(GodzillaAppTest, run_problem)
{
    class TestFile : public GYMLFile {
    public:
        TestFile(App * app, Problem * prob) : GYMLFile(app) { this->problem = prob; }
    };

    class TestApp : public App {
    public:
        TestApp() : App("godzilla", MPI_COMM_WORLD) {}

        void
        set_problem(Problem * prob)
        {
            this->gyml = new TestFile(this, prob);
        }

        void
        run()
        {
            run_problem();
        }
    } app;

    const std::string & class_name = "MockProblem";
    Parameters * pars = Factory::get_parameters(class_name);
    MockProblem * prob = app.build_object<MockProblem>(class_name, "prob", pars);

    app.set_problem(prob);

    EXPECT_EQ(app.get_problem(), prob);
    EXPECT_CALL(*prob, run);
    app.run();
}
