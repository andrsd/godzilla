#include "gtest/gtest.h"
#include "godzilla/Init.h"

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    godzilla::Init init(argc, argv);
    return RUN_ALL_TESTS();
}
