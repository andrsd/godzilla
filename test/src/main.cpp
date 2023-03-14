#include "gtest/gtest.h"
#include "Init.h"

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    godzilla::Init init(argc, argv);
    return RUN_ALL_TESTS();
}
