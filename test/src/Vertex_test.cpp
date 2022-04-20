#include "Vertex.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(VertexTest, empty_1d)
{
    Vertex1D v;
    EXPECT_EQ(v.x, 0.);
}

TEST(VertexTest, spec_1d)
{
    Vertex1D v(5.);
    EXPECT_EQ(v.x, 5.);
}

TEST(VertexTest, copy_1d)
{
    Vertex1D v(5.);

    Vertex1D * cp = v.copy();
    EXPECT_EQ(cp->x, 5.);
    delete cp;
}

TEST(VertexTest, ctor_1d)
{
    Vertex1D v(5.);

    Vertex1D cp(v);
    EXPECT_EQ(cp.x, 5.);
}

//

TEST(VertexTest, empty_2d)
{
    Vertex2D v;
    EXPECT_EQ(v.x, 0.);
    EXPECT_EQ(v.y, 0.);
}

TEST(VertexTest, spec_2d)
{
    Vertex2D v(5., 6.);
    EXPECT_EQ(v.x, 5.);
    EXPECT_EQ(v.y, 6.);
}

TEST(VertexTest, copy_2d)
{
    Vertex2D v(5., 6.);

    Vertex2D * cp = v.copy();
    EXPECT_EQ(cp->x, 5.);
    EXPECT_EQ(cp->y, 6.);
    delete cp;
}

TEST(VertexTest, ctor_2d)
{
    Vertex2D v(5., 6.);

    Vertex2D cp(v);
    EXPECT_EQ(cp.x, 5.);
    EXPECT_EQ(cp.y, 6.);
}

//

TEST(VertexTest, empty_3d)
{
    Vertex3D v;
    EXPECT_EQ(v.x, 0.);
    EXPECT_EQ(v.y, 0.);
    EXPECT_EQ(v.z, 0.);
}

TEST(VertexTest, spec_3d)
{
    Vertex3D v(5., 6., 7.);
    EXPECT_EQ(v.x, 5.);
    EXPECT_EQ(v.y, 6.);
    EXPECT_EQ(v.z, 7.);
}

TEST(VertexTest, copy_3d)
{
    Vertex3D v(5., 6., 7.);

    Vertex3D * cp = v.copy();
    EXPECT_EQ(cp->x, 5.);
    EXPECT_EQ(cp->y, 6.);
    EXPECT_EQ(cp->z, 7.);
    delete cp;
}

TEST(VertexTest, ctor_3d)
{
    Vertex3D v(5., 6., 7.);

    Vertex3D cp(v);
    EXPECT_EQ(cp.x, 5.);
    EXPECT_EQ(cp.y, 6.);
    EXPECT_EQ(cp.z, 7.);
}
