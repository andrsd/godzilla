#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/Mesh.h"
#include "godzilla/Problem.h"
#include "godzilla/GYMLFile.h"
#include "godzilla/LineMesh.h"
#include "ExceptionTestMacros.h"
#include <filesystem>

#if 0
using namespace godzilla;
namespace fs = std::filesystem;

namespace {

class MockProblem : public Problem {
public:
    explicit MockProblem(const Parameters & pars) : Problem(pars) {}

    MOCK_METHOD(void, create, ());
    MOCK_METHOD(void, run, ());
    MOCK_METHOD(void, solve, ());
    MOCK_METHOD(bool, converged, ());
    MOCK_METHOD(const Vector &, get_solution_vector, (), (const));
};

class TestProblem : public Problem {
public:
    explicit TestProblem(const Parameters & pars) : Problem(pars) {}

    void
    create() override
    {
    }
    void
    run() override
    {
    }
};

} // namespace

TEST(AppTest, DISABLED_run_input)
{
    fs::path input_file = fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "yml" / "simple-test.yml";
    std::vector<std::string> args = { "-i", input_file.string() };

    godzilla::Registry reg;
    REGISTER_OBJECT(reg, LineMesh);
    REGISTER_OBJECT(reg, TestProblem);

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, reg, "godzilla", args);
    app.run();

    // EXPECT_EQ(app.get_input_file_name(), GODZILLA_UNIT_TESTS_ROOT "/assets/yml/simple-test.yml");

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST(AppTest, run_input_non_existent_file)
{
    fs::path input_file =
        fs::path(GODZILLA_UNIT_TESTS_ROOT) / "assets" / "yml" / "non_existent_file.yml";
    std::vector<std::string> args = { "-i", input_file.string() };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", args);

    EXPECT_THAT_THROW_MSG(app.run(), ::testing::HasSubstr("Unable to open"));
}

TEST(AppTest, no_colors)
{
    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", { "--no-colors" });

    app.run();
    EXPECT_FALSE(Terminal::has_colors());
}

TEST(AppTest, verbose)
{
    int argc = 3;
    const char * argv[] = { "godzilla", "--verbose", "2", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    app.run();
    EXPECT_EQ(app.get_verbosity_level(), 2);
}

TEST(AppTest, check_integrity)
{
    testing::internal::CaptureStderr();
    TestApp app;
    app.get_logger()->error("error1");

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    auto out = testing::internal::GetCapturedStderr();
    EXPECT_THAT(out, testing::HasSubstr("[ERROR] error1"));
    EXPECT_THAT(out, testing::HasSubstr("1 error(s) found"));
}

TEST(AppTest, command_line_opt)
{
    class TestApp : public App {
    public:
        TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "test_godzilla_app", {}) {}

        cxxopts::Options &
        get_command_line_opts()
        {
            return App::get_command_line_opts();
        }
    } app;

    auto cmd_ln_opts = app.get_command_line_opts();
    EXPECT_THAT(cmd_ln_opts.help(), testing::HasSubstr("test_godzilla_app"));
}

TEST(AppTest, unknown_command_line_switch)
{
    int argc = 2;
    const char * argv[] = { "godzilla", "--asdf", nullptr };

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", argc, argv);

    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    try {
        app.run();
        FAIL();
    }
    catch (Exception & e) {
        SUCCEED();
    }
    catch (...) {
        FAIL();
    }

    testing::internal::GetCapturedStderr();
    testing::internal::GetCapturedStdout();
}

TEST(AppTest, help)
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

TEST(AppTest, version)
{
    testing::internal::CaptureStdout();

    mpi::Communicator comm(MPI_COMM_WORLD);
    App app(comm, "godzilla", { "--version" });

    app.run();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("godzilla, version"));
}
#endif
