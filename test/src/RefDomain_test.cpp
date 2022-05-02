#include "RefDomain.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(RefDomainTest, edge)
{
    auto vtcs = RefEdge::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
}

TEST(RefDomainTest, tri)
{
    auto vtcs = RefTri::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[0].y, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
    EXPECT_EQ(vtcs[1].y, -1.);
    EXPECT_EQ(vtcs[2].x, -1.);
    EXPECT_EQ(vtcs[2].y, 1.);

    auto edge_vtcs = RefTri::get_edge_vertices(0);
    EXPECT_EQ(edge_vtcs[0], 0);
    EXPECT_EQ(edge_vtcs[1], 1);
}

TEST(RefDomainTest, quad)
{
    auto vtcs = RefQuad::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[0].y, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
    EXPECT_EQ(vtcs[1].y, -1.);
    EXPECT_EQ(vtcs[2].x, 1.);
    EXPECT_EQ(vtcs[2].y, 1.);
    EXPECT_EQ(vtcs[3].x, -1.);
    EXPECT_EQ(vtcs[3].y, 1.);

    auto edge_vtcs = RefQuad::get_edge_vertices(0);
    EXPECT_EQ(edge_vtcs[0], 0);
    EXPECT_EQ(edge_vtcs[1], 1);
}

TEST(RefDomainTest, tetra)
{
    auto vtcs = RefTetra::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[0].y, -1.);
    EXPECT_EQ(vtcs[0].z, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
    EXPECT_EQ(vtcs[1].y, -1.);
    EXPECT_EQ(vtcs[1].z, -1.);
    EXPECT_EQ(vtcs[2].x, -1.);
    EXPECT_EQ(vtcs[2].y, 1.);
    EXPECT_EQ(vtcs[2].z, -1.);
    EXPECT_EQ(vtcs[3].x, -1.);
    EXPECT_EQ(vtcs[3].y, -1.);
    EXPECT_EQ(vtcs[3].z, 1.);

    auto edge_vtcs = RefTetra::get_edge_vertices(0);
    EXPECT_EQ(edge_vtcs[0], 0);
    EXPECT_EQ(edge_vtcs[1], 1);

    EXPECT_EQ(RefTetra::get_edge_orientations(), 2);
    EXPECT_EQ(RefTetra::get_num_face_vertices(0), 3);
    EXPECT_EQ(RefTetra::get_num_face_edges(0), 3);
    EXPECT_EQ(RefTetra::get_face_mode(0), MODE_TRIANGLE);

    auto face_vtcs = RefTetra::get_face_vertices(0);
    EXPECT_EQ(face_vtcs[0], 0);
    EXPECT_EQ(face_vtcs[1], 1);
    EXPECT_EQ(face_vtcs[2], 3);

    auto face_edges = RefTetra::get_face_edges(0);
    EXPECT_EQ(face_edges[0], 0);
    EXPECT_EQ(face_edges[1], 4);
    EXPECT_EQ(face_edges[2], 3);

    EXPECT_EQ(RefTetra::get_face_orientations(0), 6);

    auto face_norm = RefTetra::get_face_normal(0);
    EXPECT_EQ(face_norm.x, 0);
    EXPECT_EQ(face_norm.y, -1);
    EXPECT_EQ(face_norm.z, 0);
}

TEST(RefDomainTest, hex)
{
    auto vtcs = RefHex::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[0].y, -1.);
    EXPECT_EQ(vtcs[0].z, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
    EXPECT_EQ(vtcs[1].y, -1.);
    EXPECT_EQ(vtcs[1].z, -1.);
    EXPECT_EQ(vtcs[2].x, 1.);
    EXPECT_EQ(vtcs[2].y, 1.);
    EXPECT_EQ(vtcs[2].z, -1.);
    EXPECT_EQ(vtcs[3].x, -1.);
    EXPECT_EQ(vtcs[3].y, 1.);
    EXPECT_EQ(vtcs[3].z, -1.);
    EXPECT_EQ(vtcs[4].x, -1.);
    EXPECT_EQ(vtcs[4].y, -1.);
    EXPECT_EQ(vtcs[4].z, 1.);
    EXPECT_EQ(vtcs[5].x, 1.);
    EXPECT_EQ(vtcs[5].y, -1.);
    EXPECT_EQ(vtcs[5].z, 1.);
    EXPECT_EQ(vtcs[6].x, 1.);
    EXPECT_EQ(vtcs[6].y, 1.);
    EXPECT_EQ(vtcs[6].z, 1.);
    EXPECT_EQ(vtcs[7].x, -1.);
    EXPECT_EQ(vtcs[7].y, 1.);
    EXPECT_EQ(vtcs[7].z, 1.);

    auto edge_vtcs = RefHex::get_edge_vertices(0);
    EXPECT_EQ(edge_vtcs[0], 0);
    EXPECT_EQ(edge_vtcs[1], 1);

    EXPECT_EQ(RefHex::get_edge_orientations(), 2);
    EXPECT_EQ(RefHex::get_num_face_vertices(0), 4);
    EXPECT_EQ(RefHex::get_num_face_edges(0), 4);
    EXPECT_EQ(RefHex::get_face_mode(0), MODE_QUAD);

    auto face_vtcs = RefHex::get_face_vertices(0);
    EXPECT_EQ(face_vtcs[0], 0);
    EXPECT_EQ(face_vtcs[1], 3);
    EXPECT_EQ(face_vtcs[2], 7);
    EXPECT_EQ(face_vtcs[3], 4);

    auto face_edges = RefHex::get_face_edges(0);
    EXPECT_EQ(face_edges[0], 3);
    EXPECT_EQ(face_edges[1], 7);
    EXPECT_EQ(face_edges[2], 11);
    EXPECT_EQ(face_edges[3], 4);

    auto face_oris = RefHex::get_face_orientations(0);
    EXPECT_EQ(face_oris, 8);

    EXPECT_EQ(RefHex::get_edge_tangent_direction(0), 0);
    EXPECT_EQ(RefHex::get_face_tangent_direction(0, 0), 1);
}

TEST(RefDomainTest, prism)
{
    auto vtcs = RefPrism::get_vertices();
    EXPECT_EQ(vtcs[0].x, -1.);
    EXPECT_EQ(vtcs[0].y, -1.);
    EXPECT_EQ(vtcs[0].z, -1.);
    EXPECT_EQ(vtcs[1].x, 1.);
    EXPECT_EQ(vtcs[1].y, -1.);
    EXPECT_EQ(vtcs[1].z, -1.);
    EXPECT_EQ(vtcs[2].x, -1.);
    EXPECT_EQ(vtcs[2].y, 1.);
    EXPECT_EQ(vtcs[2].z, -1.);
    EXPECT_EQ(vtcs[3].x, -1.);
    EXPECT_EQ(vtcs[3].y, -1.);
    EXPECT_EQ(vtcs[3].z, 1.);
    EXPECT_EQ(vtcs[4].x, 1.);
    EXPECT_EQ(vtcs[4].y, -1.);
    EXPECT_EQ(vtcs[4].z, 1.);
    EXPECT_EQ(vtcs[5].x, -1.);
    EXPECT_EQ(vtcs[5].y, 1.);
    EXPECT_EQ(vtcs[5].z, 1.);

    auto edge_vtcs = RefPrism::get_edge_vertices(0);
    EXPECT_EQ(edge_vtcs[0], 0);
    EXPECT_EQ(edge_vtcs[1], 1);

    EXPECT_EQ(RefPrism::get_edge_orientations(), 2);
    EXPECT_EQ(RefPrism::get_num_face_vertices(0), 4);
    EXPECT_EQ(RefPrism::get_num_face_edges(0), 4);
    EXPECT_EQ(RefPrism::get_face_mode(0), MODE_QUAD);

    auto face_vtcs = RefPrism::get_face_vertices(0);
    EXPECT_EQ(face_vtcs[0], 0);
    EXPECT_EQ(face_vtcs[1], 1);
    EXPECT_EQ(face_vtcs[2], 4);
    EXPECT_EQ(face_vtcs[3], 3);

    auto face_edges = RefPrism::get_face_edges(0);
    EXPECT_EQ(face_edges[0], 0);
    EXPECT_EQ(face_edges[1], 4);
    EXPECT_EQ(face_edges[2], 6);
    EXPECT_EQ(face_edges[3], 3);

    EXPECT_EQ(RefPrism::get_face_orientations(0), 8);
}
