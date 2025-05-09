#include "gmock/gmock.h"
#include "godzilla/StarForest.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

TEST(StarForestTest, graph_empty)
{
    TestApp app;

    StarForest::Graph g;
    EXPECT_EQ(g.get_num_roots(), 0);
    EXPECT_EQ(g.get_num_leaves(), 0);
    EXPECT_EQ(g.get_leaves(), nullptr);
    EXPECT_EQ(g.get_remote_leaves(), nullptr);
}

TEST(StarForestTest, graph)
{
    TestApp app;

    Int n_roots = 1;
    Int n_leaves = 2;
    std::vector<Int> leaves = { 2, 3 };
    std::vector<StarForest::Node> remote_leaves(2);
    remote_leaves[0].rank = 0;
    remote_leaves[0].index = 0;
    remote_leaves[1].rank = 0;
    remote_leaves[1].index = 1;
    StarForest::Graph g(n_roots, n_leaves, leaves.data(), remote_leaves.data());
    EXPECT_EQ(g.get_num_roots(), 1);
    EXPECT_EQ(g.get_num_leaves(), 2);
    EXPECT_TRUE(g.get_leaves() != nullptr);
    EXPECT_TRUE(g.get_remote_leaves() != nullptr);
    EXPECT_EQ(g.find_leaf(3), 1);

    EXPECT_EQ(g.get_leaf(1), 3);

    EXPECT_EQ(g.get_remote_leaf(1).rank, 0);
    EXPECT_EQ(g.get_remote_leaf(1).index, 1);
}

//

TEST(StarForestTest, test)
{
    TestApp app;

    StarForest sf;
    sf.create(app.get_comm());

    Int n_leaves = 1;
    Int n_roots = 1;
    std::vector<Int> ilocal(n_leaves);
    for (Int i = 0; i < n_leaves; ++i)
        ilocal[i] = i;

    std::vector<StarForest::Node> iremote(n_leaves);
    iremote[0].rank = 0;
    iremote[0].index = 0;

    sf.set_graph(n_roots, n_leaves, ilocal, iremote);
    sf.set_up();

    auto graph = sf.get_graph();
    EXPECT_EQ(graph.get_num_roots(), n_roots);
    EXPECT_EQ(graph.get_num_leaves(), n_leaves);

    auto leaves_arr = graph.get_leaves();
    EXPECT_EQ(leaves_arr, nullptr);
    EXPECT_FALSE(static_cast<bool>(graph));

    auto remote_leaves_arr = graph.get_remote_leaves();
    EXPECT_EQ(remote_leaves_arr[0].rank, 0);
    EXPECT_EQ(remote_leaves_arr[0].index, 0);

    sf.destroy();
}

TEST(StarForestTest, reset)
{
    TestApp app;
    StarForest sf;
    sf.create(app.get_comm());
    Int n_leaves = 1;
    Int n_roots = 1;
    std::vector<Int> ilocal(n_leaves);
    for (Int i = 0; i < n_leaves; ++i)
        ilocal[i] = i;
    std::vector<StarForest::Node> iremote(n_leaves);
    iremote[0].rank = 0;
    iremote[0].index = 0;
    sf.set_graph(n_roots, n_leaves, ilocal, iremote);
    sf.reset();
    auto empty = sf.get_graph();
    EXPECT_EQ(empty.get_num_leaves(), -1);
    EXPECT_EQ(empty.get_num_roots(), -1);
    sf.destroy();
}

TEST(StarForestTest, view)
{
    testing::internal::CaptureStdout();

    TestApp app;
    StarForest sf;
    sf.create(app.get_comm());
    Int n_leaves = 1;
    Int n_roots = 1;
    std::vector<Int> ilocal(n_leaves);
    for (Int i = 0; i < n_leaves; ++i)
        ilocal[i] = i;
    std::vector<StarForest::Node> iremote(n_leaves);
    iremote[0].rank = 0;
    iremote[0].index = 0;
    sf.set_graph(n_roots, n_leaves, ilocal, iremote);
    sf.view();
    sf.destroy();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("PetscSF Object: 1 MPI process"));
    EXPECT_THAT(out, HasSubstr("type: basic"));
    EXPECT_THAT(out, HasSubstr("[0] Number of roots=1, leaves=1, remote ranks=1"));
    EXPECT_THAT(out, HasSubstr("[0] 0 <- (0,0)"));
    EXPECT_THAT(out, HasSubstr("MultiSF sort=rank-order"));
}

TEST(StarForestTest, oper_bool)
{
    TestApp app;

    StarForest sf;
    const auto & csf = sf;
    EXPECT_FALSE(sf);
    EXPECT_FALSE(csf);

    sf.create(app.get_comm());
    EXPECT_TRUE(sf);
    EXPECT_TRUE(csf);

    sf.destroy();
    EXPECT_FALSE(sf);
    EXPECT_FALSE(csf);
}
