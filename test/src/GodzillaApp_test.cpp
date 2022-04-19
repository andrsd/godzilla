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
