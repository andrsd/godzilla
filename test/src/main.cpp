#include "gtest/gtest.h"
#include "base/MooseInit.h"
#include "base/GodzillaApp.h"

PerfLog Moose::perf_log("unit-test");

int main(int argc, char **argv)
{
    MooseInit init(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    GodzillaApp::registerApps();
    return RUN_ALL_TESTS();
}
