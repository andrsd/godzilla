#include "gtest/gtest.h"
#include "mpi.h"

class MPIEnvironment : public ::testing::Environment {
public:
    virtual void
    SetUp()
    {
        char ** argv;
        int argc = 0;
        int err = MPI_Init(&argc, &argv);
        ASSERT_FALSE(err);
    }
    virtual void
    TearDown()
    {
        int err = MPI_Finalize();
        ASSERT_FALSE(err);
    }
    virtual ~MPIEnvironment() {}
};

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
    return RUN_ALL_TESTS();
}
