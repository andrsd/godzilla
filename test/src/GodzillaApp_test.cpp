#include "Common.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "petsc.h"

using namespace godzilla;

TEST_F(GodzillaAppTest, run_input_non_existent_file)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla",
                      (char *) "-i",
                      (char *) GODZILLA_UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
                      NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.create();
    app.parse_command_line(argc, argv);

    EXPECT_DEATH(app.run(), "error: Unable to open");
}

TEST_F(GodzillaAppTest, no_colors)
{
    int argc = 2;
    char * argv[] = { (char *) "godzilla", (char *) "--no-colors", NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.create();
    app.parse_command_line(argc, argv);

    app.run();
    EXPECT_EQ(Terminal::num_colors, 1);
}

TEST_F(GodzillaAppTest, verbose_level)
{
    testing::internal::CaptureStdout();

    class TestApp : public App {
    public:
        TestApp(const std::string & app_name, MPI_Comm comm) : App(app_name, comm) {}

        void
        run()
        {
            App::run();
            godzilla_print(1, "Print");
        }
    };

    int argc = 3;
    char * argv[] = { (char *) "godzilla", (char *) "--verbose", (char *) "2", NULL };

    TestApp app("godzilla", MPI_COMM_WORLD);
    app.create();
    app.parse_command_line(argc, argv);
    app.run();

    EXPECT_EQ(testing::internal::GetCapturedStdout(), "Print\n");
}

TEST_F(GodzillaAppTest, build_from_gyml)
{
    class TestApp : public App {
    public:
        TestApp(const std::string & app_name, MPI_Comm comm) : App(app_name, comm) {}
        virtual void
        build_from_gyml(const std::string & file_name)
        {
            App::build_from_gyml(file_name);
        }
        virtual void
        check_integrity()
        {
            App::check_integrity();
        }
        virtual void
        run_problem()
        {
            App::run_problem();
        }
    };

    TestApp app("godzilla", MPI_COMM_WORLD);

    // If GYMLFile was dynamically created insode App, we can use a MockGYMLFile and actually test
    // that methods are being called
    app.build_from_gyml(GODZILLA_UNIT_TESTS_ROOT "/assets/param_types.yml");
    app.check_integrity();
    app.run_problem();
}

TEST_F(GodzillaAppTest, run_input_file)
{
    class TestApp : public App {
    public:
        TestApp(const std::string & app_name, MPI_Comm comm) : App(app_name, comm) {}
        virtual void
        run_input_file(const std::string & file_name) override
        {
            App::run_input_file(file_name);
        }
        MOCK_METHOD(void, build_from_gyml, (const std::string &), (override));
        MOCK_METHOD(void, create, (), (override));
        MOCK_METHOD(void, check_integrity, (), (override));
        MOCK_METHOD(void, run_problem, (), (override));
    };

    TestApp app("godzilla", MPI_COMM_WORLD);
    EXPECT_CALL(app, build_from_gyml(testing::_)).Times(1);
    EXPECT_CALL(app, create()).Times(1);
    EXPECT_CALL(app, check_integrity()).Times(1);
    EXPECT_CALL(app, run_problem()).Times(1);
    app.run_input_file(GODZILLA_UNIT_TESTS_ROOT "/assets/param_types.yml");
}
