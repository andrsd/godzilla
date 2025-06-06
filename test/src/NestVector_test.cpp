#include "gmock/gmock.h"
#include "godzilla/NestVector.h"
#include "mpicpp-lite/mpicpp-lite.h"

using namespace godzilla;
using namespace testing;
namespace mpi = mpicpp_lite;

TEST(NestVector, test)
{
    auto v1 = Vector::create_seq(MPI_COMM_WORLD, 3);
    v1.set_values({ 0, 1, 2 }, { 5, 9, 3 });
    v1.assemble();

    auto v2 = Vector::create_seq(MPI_COMM_WORLD, 4);
    v2.set_values({ 0, 1, 2, 3 }, { 2, 4, 6, 8 });
    v2.assemble();

    auto nv = Vector::create_nest(MPI_COMM_WORLD, { v1, v2 });
    EXPECT_EQ(nv.get_nest_size(), 2);
    EXPECT_EQ(nv.get_size(), 7);

    auto sx1 = nv.get_sub_vector(0);
    EXPECT_EQ(sx1.get_size(), 3);
    std::vector<Real> sx1_vals(3);
    sx1.get_values({ 0, 1, 2 }, sx1_vals);
    EXPECT_THAT(sx1_vals, ElementsAre(5, 9, 3));

    auto sx2 = nv.get_sub_vector(1);
    EXPECT_EQ(sx2.get_size(), 4);
    std::vector<Real> sx2_vals(4);
    sx2.get_values({ 0, 1, 2, 3 }, sx2_vals);
    EXPECT_THAT(sx2_vals, ElementsAre(2, 4, 6, 8));

    auto sxs = nv.get_sub_vectors();
    EXPECT_EQ(sxs[0].get_size(), 3);
    EXPECT_EQ(sxs[1].get_size(), 4);

    v1.destroy();
    v2.destroy();
    nv.destroy();
}

TEST(NestVector, update)
{
    auto v1 = Vector::create_seq(MPI_COMM_WORLD, 3);
    v1.zero();
    v1.assemble();

    auto v2 = Vector::create_seq(MPI_COMM_WORLD, 4);
    v2.zero();
    v2.assemble();

    auto nv = Vector::create_nest(MPI_COMM_WORLD, { v1, v2 });
    v1.set_values({ 0, 1, 2 }, { 3, 5, 7 });
    v2.set_values({ 0, 1, 2, 3 }, { 2, 4, 6, 8 });
    nv.assemble();

    auto sxs = nv.get_sub_vectors();
    std::vector<Real> vals1(3);
    sxs[0].get_values({ 0, 1, 2 }, vals1);
    EXPECT_THAT(vals1, ElementsAre(3, 5, 7));

    std::vector<Real> vals2(4);
    sxs[1].get_values({ 0, 1, 2, 3 }, vals2);
    EXPECT_THAT(vals2, ElementsAre(2, 4, 6, 8));

    v1.destroy();
    v2.destroy();
    nv.destroy();
}
