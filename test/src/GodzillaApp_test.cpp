#include "GodzillaConfig.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "Mesh.h"
#include "Problem.h"
#include "GYMLFile.h"

using namespace godzilla;

namespace {

class MockProblem : public Problem {
public:
    explicit MockProblem(const Parameters & params) : Problem(params) {}

    MOCK_METHOD(void, create, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, solve, ());
    MOCK_METHOD(bool, converged, ());
    MOCK_METHOD(const Vector &, get_solution_vector, (), (const));
};

} // namespace

REGISTER_OBJECT(MockProblem);

TEST_F(GodzillaAppTest, run_input)
{
    int argc = 3;
    const char * argv[] = { "godzilla",
                            "-i",
                            GODZILLA_UNIT_TESTS_ROOT "/assets/simple.yml",
                            nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);
    app.run();

    EXPECT_EQ(app.get_input_file_name(), GODZILLA_UNIT_TESTS_ROOT "/assets/simple.yml");

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST_F(GodzillaAppTest, run_input_non_existent_file)
{
    int argc = 3;
    const char * argv[] = { "godzilla",
                            "-i",
                            GODZILLA_UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
                            nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    EXPECT_DEATH(app.run(), "\\[ERROR\\] Unable to open");
}

TEST_F(GodzillaAppTest, no_colors)
{
    int argc = 2;
    const char * argv[] = { "godzilla", "--no-colors", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    app.run();
    EXPECT_EQ(Terminal::num_colors, 1);
}

TEST_F(GodzillaAppTest, verbose)
{
    int argc = 3;
    const char * argv[] = { "godzilla", "--verbose", "2", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    app.run();
    EXPECT_EQ(app.verbosity_level(), 2);
}

TEST_F(GodzillaAppTest, check_integrity)
{
    class TestApp : public App {
    public:
        TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla") {}

        void
        run()
        {
            this->yml = allocate_input_file();
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
        TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla") {}

        void
        set_problem(Problem * prob)
        {
            this->yml = new TestFile(this, prob);
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

TEST_F(GodzillaAppTest, unknown_command_line_switch)
{
    int argc = 2;
    const char * argv[] = { "godzilla", "--asdf", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    EXPECT_DEATH(app.run(), "Error: Option ‘asdf’ does not exist");
}

TEST_F(GodzillaAppTest, help)
{
    testing::internal::CaptureStdout();

    int argc = 2;
    const char * argv[] = { "godzilla", "--help", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    app.run();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("Usage:"));
    EXPECT_THAT(out, testing::HasSubstr("godzilla [OPTION...]"));
}

TEST_F(GodzillaAppTest, version)
{
    testing::internal::CaptureStdout();

    int argc = 2;
    const char * argv[] = { "godzilla", "--version", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    app.run();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("godzilla, version"));
}
