#include "gtest/gtest.h"
#include "base/GodzillaInit.h"
#include "base/GodzillaApp.h"
#include "Moose.h"

PerfLog Moose::perf_log("unit-test");

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    GodzillaInit init(argc, argv);
    GodzillaApp::registerApps();
    return RUN_ALL_TESTS();
}
