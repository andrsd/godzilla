#include <gmock/gmock.h>
#include "godzilla/Convert.h"
#include "godzilla/Enums.h"
#include "godzilla/KrylovSolver.h"

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

TEST(ConvertTest, element_type)
{
    EXPECT_EQ(conv::to_str(EDGE2), "EDGE2");
    EXPECT_EQ(conv::to_str(TRI3), "TRI3");
    EXPECT_EQ(conv::to_str(QUAD4), "QUAD4");
    EXPECT_EQ(conv::to_str(TET4), "TET4");
    EXPECT_EQ(conv::to_str(HEX8), "HEX8");
}

TEST(ConvertTest, converged_reason)
{
    EXPECT_EQ(conv::to_str(KrylovSolver::CONVERGED_ITS), "maximum iterations");
    EXPECT_EQ(conv::to_str(KrylovSolver::DIVERGED_NULL), "null");
}
