#include "Common.h"
#include "Determinant.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(DeterminantTest, det2)
{
    EXPECT_DOUBLE_EQ(det2(1, 2, 3, 4), -2);
}

TEST(DeterminantTest, det3)
{
    EXPECT_DOUBLE_EQ(det3(1, 2, 3, 4, 5, 6, 4, 3, 2), 0);
}

TEST(DeterminantTest, det4)
{
    EXPECT_DOUBLE_EQ(det4(1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1), 0);
}

TEST(DeterminantTest, m1x1)
{
    Real1x1 m1;
    m1[0][0] = 1;
    EXPECT_DOUBLE_EQ(det(m1), 1.);
}

TEST(DeterminantTest, m2x2)
{
    Real2x2 m;
    m[0][0] = 1;
    m[0][1] = 2;
    m[1][0] = 3;
    m[1][1] = 4;
    EXPECT_DOUBLE_EQ(det(m), -2.);
}

TEST(DeterminantTest, m3x3)
{
    Real3x3 m;
    m[0][0] = 1;
    m[0][1] = 2;
    m[0][2] = 3;
    m[1][0] = 4;
    m[1][1] = 5;
    m[1][2] = 6;
    m[2][0] = 7;
    m[2][1] = 8;
    m[2][2] = 9;
    EXPECT_DOUBLE_EQ(det(m), 0.);
}

TEST(DeterminantTest, dot_product_3d)
{
    Point3D a(1, 2, 3);
    Point3D b(4, 5, 6);
    EXPECT_DOUBLE_EQ(dot_product(a, b), 32.);
}

TEST(DeterminantTest, cross_product_3d)
{
    Point3D a(1, 2, 3);
    Point3D b(4, 5, 6);
    Point3D cp = cross_product(a, b);
    EXPECT_DOUBLE_EQ(cp.x, -3.);
    EXPECT_DOUBLE_EQ(cp.y, 6.);
    EXPECT_DOUBLE_EQ(cp.z, -3.);
}

TEST(DeterminantTest, norm)
{
    Point3D a(1, 2, 4);
    EXPECT_DOUBLE_EQ(norm(a), 4.5825756949558398);
}

TEST(DeterminantTest, normalize)
{
    Point3D a(1, 2, 3);
    Point3D unit = normalize(a);
    EXPECT_DOUBLE_EQ(unit.x, 0.2672612419124244);
    EXPECT_DOUBLE_EQ(unit.y, 0.53452248382484879);
    EXPECT_DOUBLE_EQ(unit.z, 0.80178372573727319);
}
