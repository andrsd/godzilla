#include "gmock/gmock.h"
#include "godzilla/DependencyGraph.h"

using namespace godzilla;
using namespace testing;

TEST(DependencyGraph, add_node)
{
    DependencyGraph<int> gr;
    gr.add_node(2);
    EXPECT_TRUE(gr.has_node(2));
    EXPECT_FALSE(gr.has_node(10));
}

TEST(DependencyGraph, add_edge)
{
    DependencyGraph<int> gr;
    gr.add_edge(2, 4);
    EXPECT_TRUE(gr.has_node(2));
    EXPECT_TRUE(gr.has_node(4));
    EXPECT_TRUE(gr.has_edge(2, 4));
    EXPECT_FALSE(gr.has_edge(4, 2));
    EXPECT_FALSE(gr.has_edge(3, 2));
}

TEST(DependencyGraph, clear)
{
    DependencyGraph<int> gr;
    gr.add_node(2);
    gr.clear();
    EXPECT_FALSE(gr.has_node(2));
}

TEST(DependencyGraph, dfs)
{
    DependencyGraph<int> gr;
    gr.add_edge(1, 2);
    gr.add_edge(2, 3);
    gr.add_edge(2, 4);

    gr.add_edge(1, 5);
    gr.add_edge(5, 6);
    gr.add_edge(6, 7);

    gr.add_edge(5, 8);

    auto n = gr.dfs({ 1 });
    EXPECT_THAT(n, ElementsAre(1, 5, 8, 6, 7, 2, 4, 3));

    // make sure calling dfs() twice in a row will produce the same answer
    auto m = gr.dfs({ 1 });
    EXPECT_THAT(m, ElementsAre(1, 5, 8, 6, 7, 2, 4, 3));
}

TEST(DependencyGraph, dfs_cyclic)
{
    DependencyGraph<int> gr;
    gr.add_edge(1, 2);
    gr.add_edge(2, 3);
    gr.add_edge(3, 1);
    EXPECT_THROW({ gr.dfs({ 1 }); }, std::runtime_error);
}

TEST(DependencyGraph, bfs)
{
    DependencyGraph<int> gr;
    gr.add_edge(1, 2);
    gr.add_edge(2, 3);
    gr.add_edge(2, 4);

    gr.add_edge(1, 5);
    gr.add_edge(5, 6);
    gr.add_edge(6, 7);

    gr.add_edge(5, 8);

    auto n = gr.bfs({ 1 });
    EXPECT_THAT(n, ElementsAre(1, 2, 5, 3, 4, 6, 8, 7));

    // make sure calling bfs() twice in a row will produce the same answer
    auto m = gr.dfs({ 1 });
    EXPECT_THAT(m, ElementsAre(1, 5, 8, 6, 7, 2, 4, 3));
}

TEST(DependencyGraph, bfs_cyclic)
{
    DependencyGraph<int> gr;
    gr.add_edge(1, 2);
    gr.add_edge(2, 3);
    gr.add_edge(3, 1);
    EXPECT_THROW({ gr.bfs({ 1 }); }, std::runtime_error);
}
