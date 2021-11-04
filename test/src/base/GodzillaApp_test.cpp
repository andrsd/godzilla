#include "GodzillaConfig.h"
#include <gmock/gmock.h>
#include "base/GodzillaApp_test.h"
#include "parser/CommandLine.h"


TEST_F(GodzillaAppTest, print_version)
{
    testing::internal::CaptureStdout();

    int argc = 2;
    char *argv[] = {
        (char *) "godzilla",
        (char *) "--version",
        NULL
    };

    auto command_line = std::make_shared<CommandLine>(argc, argv);
    auto comm = std::make_shared<Parallel::Communicator>(MPI_COMM_WORLD);

    InputParameters params = emptyInputParameters();
    params.set<std::shared_ptr<Parallel::Communicator>>("_comm") = comm;
    params.set<std::string>("_type") = "GodzillaApp";
    params.set<std::string>("_app_name") = "godzilla";
    params.set<bool>("perf_graph_live_all") = false;
    params.set<bool>("disable_perf_graph_live") = true;
    params.set<bool>("use_split") = true;
    params.set<bool>("check_input") = true;
    params.set<bool>("automatic_automatic_scaling") = false;
    params.set<int>("_argc") = argc;
    params.set<char **>("_argv") = argv;
    params.set<std::shared_ptr<CommandLine>>("_command_line") = command_line;
    params.addParam<unsigned int>("stop_for_debugger", 30, "A");
    params.set<unsigned int>("verbosity_level") = 1;
    params.set<bool>("help") = false;
    params.set<bool>("display_version") = true;

    command_line->addCommandLineOptionsFromParams(params);
    command_line->populateInputParams(params);

    auto app = buildApp<GodzillaApp>(params);

    app->run();

    EXPECT_THAT(
        testing::internal::GetCapturedStdout(),
        testing::StartsWith("godzilla version")
    );
}

TEST_F(GodzillaAppTest, print_help)
{
    testing::internal::CaptureStdout();

    int argc = 2;
    char *argv[] = {
        (char *) "godzilla",
        (char *) "--help",
        NULL
    };

    auto command_line = std::make_shared<CommandLine>(argc, argv);
    auto comm = std::make_shared<Parallel::Communicator>(MPI_COMM_WORLD);

    InputParameters params = emptyInputParameters();
    params.set<std::shared_ptr<Parallel::Communicator>>("_comm") = comm;
    params.set<std::string>("_type") = "GodzillaApp";
    params.set<std::string>("_app_name") = "godzilla";
    params.set<bool>("perf_graph_live_all") = false;
    params.set<bool>("disable_perf_graph_live") = true;
    params.set<bool>("use_split") = true;
    params.set<bool>("check_input") = true;
    params.set<bool>("automatic_automatic_scaling") = false;
    params.set<int>("_argc") = argc;
    params.set<char **>("_argv") = argv;
    params.set<std::shared_ptr<CommandLine>>("_command_line") = command_line;
    params.addParam<unsigned int>("stop_for_debugger", 30, "A");
    params.set<unsigned int>("verbosity_level") = 1;
    params.set<bool>("help") = true;
    params.set<bool>("display_version") = false;

    command_line->addCommandLineOptionsFromParams(params);
    command_line->populateInputParams(params);

    auto app = buildApp<GodzillaApp>(params);

    app->run();

    EXPECT_THAT(
        testing::internal::GetCapturedStdout(),
        testing::StartsWith("Usage")
    );
}

TEST_F(GodzillaAppTest, run_input)
{
    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/simple.yml");

    int argc = 3;
    char *argv[] = {
        (char *) "godzilla",
        (char *) "-i",
        (char *) UNIT_TESTS_ROOT "/assets/simple.yml",
        NULL
    };

    auto command_line = std::make_shared<CommandLine>(argc, argv);
    auto comm = std::make_shared<Parallel::Communicator>(MPI_COMM_WORLD);

    InputParameters params = emptyInputParameters();
    params.set<std::shared_ptr<Parallel::Communicator>>("_comm") = comm;
    params.set<std::string>("_type") = "GodzillaApp";
    params.set<std::string>("_app_name") = "godzilla";
    params.set<bool>("perf_graph_live_all") = false;
    params.set<bool>("disable_perf_graph_live") = true;
    params.set<bool>("use_split") = true;
    params.set<bool>("check_input") = true;
    params.set<bool>("automatic_automatic_scaling") = false;
    params.set<int>("_argc") = argc;
    params.set<char **>("_argv") = argv;
    params.set<std::shared_ptr<CommandLine>>("_command_line") = command_line;
    params.addParam<unsigned int>("stop_for_debugger", 30, "A");
    params.set<unsigned int>("verbosity_level") = 1;
    params.set<bool>("help") = false;
    params.set<bool>("display_version") = false;
    params.set<std::string>("input_file") = file_name;

    command_line->addCommandLineOptionsFromParams(params);
    command_line->populateInputParams(params);

    auto app = buildApp<GodzillaApp>(params);

    app->run();

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST_F(GodzillaAppTest, run_input_non_existen_file)
{
    std::string file_name =
        std::string(UNIT_TESTS_ROOT) + std::string("/assets/non_existent_file.yml");

    int argc = 3;
    char *argv[] = {
        (char *) "godzilla",
        (char *) "-i",
        (char *) UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
        NULL
    };

    auto command_line = std::make_shared<CommandLine>(argc, argv);
    auto comm = std::make_shared<Parallel::Communicator>(MPI_COMM_WORLD);

    InputParameters params = emptyInputParameters();
    params.set<std::shared_ptr<Parallel::Communicator>>("_comm") = comm;
    params.set<std::string>("_type") = "GodzillaApp";
    params.set<std::string>("_app_name") = "godzilla";
    params.set<bool>("perf_graph_live_all") = false;
    params.set<bool>("disable_perf_graph_live") = true;
    params.set<bool>("use_split") = true;
    params.set<bool>("check_input") = true;
    params.set<bool>("automatic_automatic_scaling") = false;
    params.set<int>("_argc") = argc;
    params.set<char **>("_argv") = argv;
    params.set<std::shared_ptr<CommandLine>>("_command_line") = command_line;
    params.addParam<unsigned int>("stop_for_debugger", 30, "A");
    params.set<unsigned int>("verbosity_level") = 1;
    params.set<bool>("help") = false;
    params.set<bool>("display_version") = false;
    params.set<std::string>("input_file") = file_name;

    command_line->addCommandLineOptionsFromParams(params);
    command_line->populateInputParams(params);

    auto app = buildApp<GodzillaApp>(params);

    EXPECT_DEATH(
        app->run(),
        "ERROR: Unable to open"
    );
}
