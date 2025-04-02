#include <gmock/gmock.h>
#include "godzilla/Convert.h"
#include "godzilla/Enums.h"

using namespace godzilla;

TEST(ConvertTest, polytope_type)
{
    EXPECT_EQ(conv::to_str(PolytopeType::POINT), "POINT");
    EXPECT_EQ(conv::to_str(PolytopeType::SEGMENT), "SEGMENT");
    EXPECT_EQ(conv::to_str(PolytopeType::POINT_PRISM_TENSOR), "POINT_PRISM_TENSOR");
    EXPECT_EQ(conv::to_str(PolytopeType::TRIANGLE), "TRIANGLE");
    EXPECT_EQ(conv::to_str(PolytopeType::QUADRILATERAL), "QUADRILATERAL");
    EXPECT_EQ(conv::to_str(PolytopeType::SEG_PRISM_TENSOR), "SEG_PRISM_TENSOR");
    EXPECT_EQ(conv::to_str(PolytopeType::TETRAHEDRON), "TETRAHEDRON");
    EXPECT_EQ(conv::to_str(PolytopeType::HEXAHEDRON), "HEXAHEDRON");
    EXPECT_EQ(conv::to_str(PolytopeType::TRI_PRISM), "TRI_PRISM");
    EXPECT_EQ(conv::to_str(PolytopeType::TRI_PRISM_TENSOR), "TRI_PRISM_TENSOR");
    EXPECT_EQ(conv::to_str(PolytopeType::QUAD_PRISM_TENSOR), "QUAD_PRISM_TENSOR");
    EXPECT_EQ(conv::to_str(PolytopeType::PYRAMID), "PYRAMID");
    EXPECT_EQ(conv::to_str(PolytopeType::FV_GHOST), "FV_GHOST");
    EXPECT_EQ(conv::to_str(PolytopeType::INTERIOR_GHOST), "INTERIOR_GHOST");
    EXPECT_EQ(conv::to_str(PolytopeType::UNKNOWN), "UNKNOWN");
}
