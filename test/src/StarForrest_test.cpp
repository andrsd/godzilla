#include "gmock/gmock.h"
#include "godzilla/StarForest.h"
#include "TestApp.h"

using namespace godzilla;

TEST(StarForestTest, test)
{
    TestApp app;

    StarForest sf;
    sf.create(app.get_comm());

    Int n_leaves = 1;
    Int n_roots = 1;
    std::vector<Int> ilocal(n_leaves);
    for (Int i = 0; i < n_leaves; i++)
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

    auto remote_leaves_arr = graph.get_remote_leaves();
    EXPECT_EQ(remote_leaves_arr[0].rank, 0);
    EXPECT_EQ(remote_leaves_arr[0].index, 0);

    sf.destroy();
}
