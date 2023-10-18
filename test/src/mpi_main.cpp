#include "gtest/gtest.h"
#include "mpi.h"

class MPIEnvironment : public ::testing::Environment {
public:
    void
    SetUp() override
    {
        char ** argv;
        int argc = 0;
        int err = MPI_Init(&argc, &argv);
        ASSERT_FALSE(err);
    }

    void
    TearDown() override
    {
        int err = MPI_Finalize();
        ASSERT_FALSE(err);
    }

    ~MPIEnvironment() override {}
};

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
    return RUN_ALL_TESTS();
}
