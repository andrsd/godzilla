#include "RefDomain.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(RefDomainTest, edge)
{
    auto vtcs = RefEdge::get_vertices();
}

TEST(RefDomainTest, tri)
{
    auto vtcs = RefTri::get_vertices();
    auto edge_vtcs = RefTri::get_edge_vertices(0);
}

TEST(RefDomainTest, quad)
{
    auto vtcs = RefQuad::get_vertices();
    auto edge_vtcs = RefQuad::get_edge_vertices(0);
}

TEST(RefDomainTest, tetra)
{
    auto vtcs = RefTetra::get_vertices();
    auto edge_vtcs = RefTetra::get_edge_vertices(0);
    EXPECT_EQ(RefTetra::get_edge_orientations(), 2);
    EXPECT_EQ(RefTetra::get_num_face_vertices(0), 3);
    EXPECT_EQ(RefTetra::get_num_face_edges(0), 3);
    EXPECT_EQ(RefTetra::get_face_mode(0), MODE_TRIANGLE);
    auto face_vtcs = RefTetra::get_face_vertices(0);
    auto face_edges = RefTetra::get_face_edges(0);
    auto face_oris = RefTetra::get_face_orientations(0);
    auto face_norm = RefTetra::get_face_normal(0);
}

TEST(RefDomainTest, hex)
{
    auto vtcs = RefHex::get_vertices();
    auto edge_vtcs = RefHex::get_edge_vertices(0);
    EXPECT_EQ(RefHex::get_edge_orientations(), 2);
    EXPECT_EQ(RefHex::get_num_face_vertices(0), 4);
    EXPECT_EQ(RefHex::get_num_face_edges(0), 4);
    EXPECT_EQ(RefHex::get_face_mode(0), MODE_QUAD);
    auto face_vtcs = RefHex::get_face_vertices(0);
    auto face_edges = RefHex::get_face_edges(0);
    auto face_oris = RefHex::get_face_orientations(0);
    EXPECT_EQ(RefHex::get_edge_tangent_direction(0), 0);
    EXPECT_EQ(RefHex::get_face_tangent_direction(0, 0), 1);
}

TEST(RefDomainTest, prism)
{
    auto vtcs = RefPrism::get_vertices();
    auto edge_vtcs = RefPrism::get_edge_vertices(0);
    EXPECT_EQ(RefPrism::get_edge_orientations(), 2);
    EXPECT_EQ(RefPrism::get_num_face_vertices(0), 4);
    EXPECT_EQ(RefPrism::get_num_face_edges(0), 4);
    EXPECT_EQ(RefPrism::get_face_mode(0), MODE_QUAD);
    auto face_vtcs = RefPrism::get_face_vertices(0);
    auto face_edges = RefPrism::get_face_edges(0);
    auto face_oris = RefPrism::get_face_orientations(0);
}
