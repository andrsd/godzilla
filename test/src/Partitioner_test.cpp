#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/Partitioner.h"

using namespace godzilla;
using namespace testing;

TEST(PartitionerTest, null_ctor)
{
    Partitioner part;
    auto p = (PetscPartitioner) part;
    EXPECT_EQ(p, nullptr);
}

TEST(PartitionerTest, mpi_ctor)
{
    TestApp app;
    Partitioner part(app.get_comm());
    auto p = (PetscPartitioner) part;
    EXPECT_NE(p, nullptr);
}

#ifdef PETSC_HAVE_PTSCOTCH
TEST(PartitionerTest, ptscotch)
{
    TestApp app;
    Partitioner part(app.get_comm());
    part.set_type("ptscotch");
    EXPECT_EQ(part.get_type(), "ptscotch");
}
#endif

TEST(PartitionerTest, set_up)
{
    TestApp app;
    Partitioner part(app.get_comm());
    part.set_up();
}

TEST(PartitionerTest, reset)
{
    TestApp app;
    Partitioner part(app.get_comm());
    part.reset();
}

TEST(PartitionerTest, view)
{
    testing::internal::CaptureStdout();
    TestApp app;
    Partitioner part(app.get_comm());
    part.view();
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("type: simple"));
}
