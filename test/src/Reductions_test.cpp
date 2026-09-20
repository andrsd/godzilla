#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/Reductions.h"
#include "godzilla/Range.h"

using namespace godzilla;

TEST(ReductionsTest, for_each_index_set)
{
    TestApp app;
    auto comm = app.get_comm();

    auto is = IndexSet::create_stride(comm, 5, 0, 2);

    Int total;
    auto sumr = reduction(total, 3, mpi::op::sum<Int>());
    for_each(comm, is, sumr, [&](Int i, Int val, auto & reducer) {
        //
        reducer.combine(val);
    });

    EXPECT_EQ(total, 23);
}

TEST(ReductionsTest, range)
{
    TestApp app;
    auto comm = app.get_comm();

    auto rng = make_range(5);

    Int total;
    auto sumr = reduction(total, 3, mpi::op::sum<Int>());
    for_each(comm, rng, sumr, [&](Int i, auto & reducer) {
        //
        reducer.combine(2 * i);
    });

    EXPECT_EQ(total, 23);
}
