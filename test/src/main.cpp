#include "gtest/gtest.h"
#include "Init.h"
#include "App.h"

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    godzilla::Init init(argc, argv);
    testing::GTEST_FLAG(death_test_style) = "threadsafe";
    return RUN_ALL_TESTS();
}
