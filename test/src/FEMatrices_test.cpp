#include "gmock/gmock.h"
#include "FEMatrices.h"

using namespace godzilla;

TEST(FEMatricesTest, mass_edge2)
{
    auto m = fe::matrix::mass<EDGE2>();
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), 2.);
}

TEST(FEMatricesTest, mass_tri3)
{
    auto m = fe::matrix::mass<TRI3>();
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 1.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), 1.);
    EXPECT_EQ(m(2, 0), 1.);
    EXPECT_EQ(m(2, 1), 1.);
    EXPECT_EQ(m(2, 2), 2.);
}

TEST(FEMatricesTest, mass_quad4)
{
    EXPECT_DEATH(fe::matrix::mass<QUAD4>(), "Mass matrix in not implemented for QUAD4");
}

TEST(FEMatricesTest, mass_tet4)
{
    EXPECT_DEATH(fe::matrix::mass<TET4>(), "Mass matrix in not implemented for TET4");
}

TEST(FEMatricesTest, mass_hex8)
{
    EXPECT_DEATH(fe::matrix::mass<HEX8>(), "Mass matrix in not implemented for HEX8");
}

TEST(FEMatricesTest, stiffness_edge2)
{
    auto m = fe::matrix::stiffness<EDGE2>();
    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(1, 0), -1.);
    EXPECT_EQ(m(1, 1), 1.);
}

TEST(FEMatricesTest, stiffness_tri3)
{
    auto m = fe::matrix::stiffness<TRI3>();
    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), -1.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), -1.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), -1.);
    EXPECT_EQ(m(2, 2), 1);
}

TEST(FEMatricesTest, stiffness_quad4)
{
    EXPECT_DEATH(fe::matrix::stiffness<QUAD4>(), "Stiffness matrix in not implemented for QUAD4");
}

TEST(FEMatricesTest, stiffness_tet4)
{
    EXPECT_DEATH(fe::matrix::stiffness<TET4>(), "Stiffness matrix in not implemented for TET4");
}

TEST(FEMatricesTest, stiffness_hex8)
{
    EXPECT_DEATH(fe::matrix::stiffness<HEX8>(), "Stiffness matrix in not implemented for HEX8");
}
