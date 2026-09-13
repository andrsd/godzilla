#include "gmock/gmock.h"
#include "godzilla/BoundingBox.h"
#include "godzilla/Span.h"

using namespace godzilla;

TEST(BoundingBoxTest, ctor_default)
{
    BoundingBox<3_D> bbox;

    EXPECT_NEAR(bbox.min[0], std::numeric_limits<Real>::max(), 1e-15);
    EXPECT_NEAR(bbox.min[1], std::numeric_limits<Real>::max(), 1e-15);
    EXPECT_NEAR(bbox.min[2], std::numeric_limits<Real>::max(), 1e-15);

    EXPECT_NEAR(bbox.max[0], std::numeric_limits<Real>::lowest(), 1e-15);
    EXPECT_NEAR(bbox.max[1], std::numeric_limits<Real>::lowest(), 1e-15);
    EXPECT_NEAR(bbox.max[2], std::numeric_limits<Real>::lowest(), 1e-15);
}

TEST(BoundingBoxTest, ctor_std_arr)
{
    std::array<Real, 3_D> lo = { -1, -2, -3 };
    std::array<Real, 3_D> hi = { 1, 2, 3 };
    BoundingBox<3_D> bbox(lo, hi);

    EXPECT_NEAR(bbox.min[0], lo[0], 1e-15);
    EXPECT_NEAR(bbox.min[1], lo[1], 1e-15);
    EXPECT_NEAR(bbox.min[2], lo[2], 1e-15);

    EXPECT_NEAR(bbox.max[0], hi[0], 1e-15);
    EXPECT_NEAR(bbox.max[1], hi[1], 1e-15);
    EXPECT_NEAR(bbox.max[2], hi[2], 1e-15);
}

TEST(BoundingBoxTest, ctor_span)
{
    std::array<Real, 3_D> lo = { -1, -2, -3 };
    std::array<Real, 3_D> hi = { 1, 2, 3 };
    BoundingBox<3_D> bbox(Span(lo.data(), 3), Span(hi.data(), 3));

    EXPECT_NEAR(bbox.min[0], -1, 1e-15);
    EXPECT_NEAR(bbox.min[1], -2, 1e-15);
    EXPECT_NEAR(bbox.min[2], -3, 1e-15);

    EXPECT_NEAR(bbox.max[0], 1, 1e-15);
    EXPECT_NEAR(bbox.max[1], 2, 1e-15);
    EXPECT_NEAR(bbox.max[2], 3, 1e-15);
}

TEST(BoundingBoxTest, min_max_idx)
{
    BoundingBox<3_D> bbox({ -1, -2, -3 }, { 1, 2, 3 });

    EXPECT_NEAR(bbox.min[0], -1, 1e-15);
    EXPECT_NEAR(bbox.min[1], -2, 1e-15);
    EXPECT_NEAR(bbox.min[2], -3, 1e-15);

    EXPECT_NEAR(bbox.max[0], 1, 1e-15);
    EXPECT_NEAR(bbox.max[1], 2, 1e-15);
    EXPECT_NEAR(bbox.max[2], 3, 1e-15);
}

TEST(BoundingBoxTest, contains)
{
    BoundingBox<3_D> bbox({ -1, -2, -3 }, { 1, 2, 3 });

    {
        std::array<Real, 3_D> point = { 0, 0, 0 };
        EXPECT_TRUE(bbox.contains(Span(point.data(), 3), 1e-10));
    }
    {
        std::array<Real, 3_D> point = { 2, 2, 3 };
        EXPECT_FALSE(bbox.contains(Span(point.data(), 3), 1e-10));
    }
}

TEST(BoundingBoxTest, create_from_points)
{
    std::vector<Real> points = { 0, 0, 1, 0, 2, 0, 2, 1, 1, -1 };
    auto bbox = BoundingBox<2_D>::create_from_points(points);

    EXPECT_NEAR(bbox.min[0], 0., 1e-15);
    EXPECT_NEAR(bbox.min[1], -1., 1e-15);
    EXPECT_NEAR(bbox.max[0], 2., 1e-15);
    EXPECT_NEAR(bbox.max[1], 1., 1e-15);
}

TEST(BoundingBoxTest, intersect)
{
    {
        BoundingBox<3_D> a({ 0, 0, 0 }, { 1, 2, 3 });
        BoundingBox<3_D> b({ -1, -2, -3 }, { 0.5, 1, 1.5 });
        EXPECT_TRUE(intersect(a, b, 1e-10));
    }

    {
        BoundingBox<3_D> a({ 1e-9, 1e-9, 1e-9 }, { 1, 2, 3 });
        BoundingBox<3_D> b({ -1, -2, -3 }, { 0., 0., 0. });
        EXPECT_TRUE(intersect(a, b, 1e-8));
        EXPECT_FALSE(intersect(a, b, 1e-10));
    }
}
