#include "GodzillaConfig.h"
#include "gmock/gmock.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

TEST_F(GodzillaAppTest, run_input)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla",
                      (char *) "-i",
                      (char *) UNIT_TESTS_ROOT "/assets/simple.yml",
                      NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.create();
    app.parseCommandLine(argc, argv);
    app.run();

    // TODO: build a MockGodzillaApp and make sure methods get called
}

TEST_F(GodzillaAppTest, run_input_non_existent_file)
{
    int argc = 3;
    char * argv[] = { (char *) "godzilla",
                      (char *) "-i",
                      (char *) UNIT_TESTS_ROOT "/assets/non_existent_file.yml",
                      NULL };

    App app("godzilla", MPI_COMM_WORLD);
    app.create();
    app.parseCommandLine(argc, argv);

    EXPECT_DEATH(app.run(), "ERROR: Unable to open");
}
