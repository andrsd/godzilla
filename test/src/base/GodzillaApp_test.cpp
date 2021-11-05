#include "GodzillaConfig.h"
#include <gmock/gmock.h>
#include "base/GodzillaApp_test.h"


using namespace godzilla;

TEST_F(GodzillaAppTest, print_version)
{
    // testing::internal::CaptureStdout();
    //
    // int argc = 2;
    // char *argv[] = {
    //     (char *) "godzilla",
    //     (char *) "--version",
    //     NULL
    // };
    //
    // App app("godzilla", MPI_COMM_WORLD);
    // app.create();
    // app.parseCommandLine(argc, argv);
    //
    //
    // EXPECT_THAT(
    //     testing::internal::GetCapturedStdout(),
    //     testing::StartsWith("godzilla version")
    // );
}

TEST_F(GodzillaAppTest, print_help)
{
    // testing::internal::CaptureStdout();
    //
    // int argc = 2;
    // char *argv[] = {
    //     (char *) "godzilla",
    //     (char *) "--help",
    //     NULL
    // };
    //
    // App app("godzilla", MPI_COMM_WORLD);
    // app.create();
    // app.parseCommandLine(argc, argv);
    // app.run();
    //
    // EXPECT_THAT(
    //     testing::internal::GetCapturedStdout(),
    //     testing::StartsWith("Usage")
    // );
}

TEST_F(GodzillaAppTest, run_input)
{
    // std::string file_name =
    //     std::string(UNIT_TESTS_ROOT) + std::string("/assets/simple.yml");
    //
    // int argc = 3;
    // char *argv[] = {
    //     (char *) "godzilla",
    //     (char *) "-i",
    //     (char *) UNIT_TESTS_ROOT "/assets/simple.yml",
    //     NULL
    // };
    //
    // App app("godzilla", MPI_COMM_WORLD);
    // app.create();
    // app.parseCommandLine(argc, argv);
    // app.run();

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST_F(GodzillaAppTest, run_input_non_existen_file)
{
    // std::string file_name =
    //     std::string(UNIT_TESTS_ROOT) + std::string("/assets/non_existent_file.yml");
    //
    // int argc = 3;
    // char *argv[] = {
    //     (char *) "godzilla",
    //     (char *) "-i",
    //     (char *) UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
    //     NULL
    // };
    //
    // App app("godzilla", MPI_COMM_WORLD);
    // app.create();
    // app.parseCommandLine(argc, argv);
    //
    // EXPECT_DEATH(
    //     app.run(),
    //     "ERROR: Unable to open"
    // );
}
