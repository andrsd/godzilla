#include <gmock/gmock.h>
#include <mpi.h>
#include "TestApp.h"
#include "godzilla/Partitioning.h"

using namespace godzilla;

namespace {

// Create the adjacency matrix for a 1D mesh with `n` elements
Matrix
create_adj_mat_1d(MPI_Comm comm, Int n)
{
    auto adj = Matrix::create_seq_aij(comm, n, n, 2);
    for (Int i = 0; i < n; ++i) {
        if (i == 0) {
            adj.set_value(0, 1, 1);
        }
        else if (i == n - 1) {
            adj.set_value(i, i - 1, 1);
        }
        else {
            adj.set_value(i, i - 1, 1);
            adj.set_value(i, i + 1, 1);
        }
    }
    adj.assemble();
    return adj;
}

} // namespace

TEST(Partitioning, partition)
{
    TestApp app;
    auto comm = app.get_comm();

    Partitioning mpart;
    mpart.create(comm);
    mpart.set_type(Partitioning::PARMETIS);
    mpart.set_n_parts(3);
    auto adj = create_adj_mat_1d(comm, 7);
    mpart.set_adjacency(adj);
    auto p = mpart.apply();

    {
        auto vals = p.borrow_indices();
        EXPECT_EQ(vals[0], 2);
        EXPECT_EQ(vals[1], 2);
        EXPECT_EQ(vals[2], 2);
        EXPECT_EQ(vals[3], 1);
        EXPECT_EQ(vals[4], 1);
        EXPECT_EQ(vals[5], 0);
        EXPECT_EQ(vals[6], 0);
    }

    p.destroy();
    mpart.destroy();
}

TEST(Partitioning, edge_weights)
{
    TestApp app;
    auto comm = app.get_comm();

    Partitioning mpart;
    mpart.create(comm);
    mpart.set_type(Partitioning::PARMETIS);
    mpart.set_n_parts(3);
    Int n = 9;
    // @note: this did not seem to have any effect when using parmetis of ptscotch :shrug:
    std::vector<Real> edge_wt = { 2, 5, 5, 5, 5, 5, 5, 5, 3 };
    auto adj = Matrix::create_seq_aij(comm, n + 1, n + 1, 2);
    for (Int i = 0; i < n; ++i) {
        adj.set_value(i, i + 1, edge_wt[i]);
        adj.set_value(i + 1, i, edge_wt[i]);
    }
    adj.assemble();
    mpart.set_adjacency(adj);
    mpart.set_use_edge_weights(true);
    auto p = mpart.apply();

    {
        auto vals = p.borrow_indices();
        EXPECT_EQ(vals[0], 1);
        EXPECT_EQ(vals[1], 1);
        EXPECT_EQ(vals[2], 1);
        EXPECT_EQ(vals[3], 2);
        EXPECT_EQ(vals[4], 2);
        EXPECT_EQ(vals[5], 2);
        EXPECT_EQ(vals[6], 2);
        EXPECT_EQ(vals[7], 0);
        EXPECT_EQ(vals[8], 0);
        EXPECT_EQ(vals[9], 0);
    }

    EXPECT_TRUE(mpart.get_use_edge_weights());

    p.destroy();
    mpart.destroy();
}

TEST(Partitioning, vertex_weights)
{
    TestApp app;
    auto comm = app.get_comm();

    Partitioning mpart;
    mpart.create(comm);
    mpart.set_type(Partitioning::PARMETIS);
    mpart.set_n_parts(2);
    auto adj = create_adj_mat_1d(comm, 5);
    mpart.set_adjacency(adj);
    mpart.set_number_vertex_weights(1);
    mpart.set_vertex_weights({ 10, 1, 1, 1, 1 });
    auto p = mpart.apply();

    {
        auto vals = p.borrow_indices();
        EXPECT_EQ(vals[0], 0);
        EXPECT_EQ(vals[1], 1);
        EXPECT_EQ(vals[2], 1);
        EXPECT_EQ(vals[3], 1);
        EXPECT_EQ(vals[4], 1);
    }

    p.destroy();
    mpart.destroy();
}

TEST(Partitioning, partition_weights)
{
    TestApp app;
    auto comm = app.get_comm();

    Partitioning mpart;
    mpart.create(comm);
    mpart.set_type(Partitioning::PARMETIS);
    mpart.set_n_parts(2);
    mpart.set_partition_weights({ 0.8, 0.2 });
    auto adj = create_adj_mat_1d(comm, 10);
    adj.assemble();
    mpart.set_adjacency(adj);
    auto p = mpart.apply();

    {
        auto vals = p.borrow_indices();
        EXPECT_EQ(vals[0], 0);
        EXPECT_EQ(vals[1], 0);
        EXPECT_EQ(vals[2], 0);
        EXPECT_EQ(vals[3], 0);
        EXPECT_EQ(vals[4], 0);
        EXPECT_EQ(vals[5], 0);
        EXPECT_EQ(vals[6], 0);
        EXPECT_EQ(vals[7], 0);
        EXPECT_EQ(vals[8], 1);
        EXPECT_EQ(vals[9], 1);
    }

    p.destroy();
    mpart.destroy();
}
