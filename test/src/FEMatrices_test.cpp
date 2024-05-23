#include "gmock/gmock.h"
#include "godzilla/FEMatrices.h"

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
    auto m = fe::matrix::mass<TET4>();
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(0, 2), 1.);
    EXPECT_EQ(m(0, 3), 1.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), 1.);
    EXPECT_EQ(m(1, 3), 1.);
    EXPECT_EQ(m(2, 0), 1.);
    EXPECT_EQ(m(2, 1), 1.);
    EXPECT_EQ(m(2, 2), 2.);
    EXPECT_EQ(m(2, 3), 1.);
    EXPECT_EQ(m(3, 0), 1.);
    EXPECT_EQ(m(3, 1), 1.);
    EXPECT_EQ(m(3, 2), 1.);
    EXPECT_EQ(m(3, 3), 2.);
}

TEST(FEMatricesTest, mass_hex8)
{
    EXPECT_DEATH(fe::matrix::mass<HEX8>(), "Mass matrix in not implemented for HEX8");
}

TEST(FEMatricesTest, mass_rz_edge2)
{
    DenseVector<Real, 2> rad_n({ 1., 3. });
    auto m = fe::matrix::mass_rz<EDGE2>(2, rad_n);
    EXPECT_EQ(m(0, 0), 6.);
    EXPECT_EQ(m(0, 1), 4.);
    EXPECT_EQ(m(1, 0), 4.);
    EXPECT_EQ(m(1, 1), 10.);
}

TEST(FEMatricesTest, mass_rz_tri3)
{
    DenseVector<Real, 3> rad_n({ 1., 3., 4. });
    auto m = fe::matrix::mass_rz<TRI3>(2, rad_n);
    EXPECT_EQ(m(0, 0), 16.);
    EXPECT_EQ(m(0, 1), 8.);
    EXPECT_EQ(m(0, 2), 9.);
    EXPECT_EQ(m(1, 0), 8.);
    EXPECT_EQ(m(1, 1), 24.);
    EXPECT_EQ(m(1, 2), 11.);
    EXPECT_EQ(m(2, 0), 9.);
    EXPECT_EQ(m(2, 1), 11.);
    EXPECT_EQ(m(2, 2), 28.);
}

TEST(FEMatricesTest, mass_lumped_edge2)
{
    auto m = fe::matrix::mass_lumped<EDGE2>();
    EXPECT_EQ(m(0, 0), 3.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 3.);
}

TEST(FEMatricesTest, mass_lumped_tri3)
{
    auto m = fe::matrix::mass_lumped<TRI3>();
    EXPECT_EQ(m(0, 0), 4.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 4.);
    EXPECT_EQ(m(1, 2), 0.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), 4.);
}

TEST(FEMatricesTest, mass_lumped_tet4)
{
    auto m = fe::matrix::mass_lumped<TET4>();
    EXPECT_EQ(m(0, 0), 5.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(0, 3), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 5.);
    EXPECT_EQ(m(1, 2), 0.);
    EXPECT_EQ(m(1, 3), 0.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), 5.);
    EXPECT_EQ(m(2, 3), 0.);
    EXPECT_EQ(m(3, 0), 0.);
    EXPECT_EQ(m(3, 1), 0.);
    EXPECT_EQ(m(3, 2), 0.);
    EXPECT_EQ(m(3, 3), 5.);
}

TEST(FEMatricesTest, mass_lumped_rz_edge2)
{
    DenseVector<Real, 2> rad_n({ 1, 2 });
    auto m = fe::matrix::mass_lumped_rz<EDGE2>(rad_n);
    EXPECT_EQ(m(0, 0), 8.);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 10.);
}

TEST(FEMatricesTest, mass_lumped_rz_tri3)
{
    DenseVector<Real, 3> rad_n({ 1, 2, 3 });
    auto m = fe::matrix::mass_lumped_rz<TRI3>(rad_n);
    EXPECT_EQ(m(0, 0), 35);
    EXPECT_EQ(m(0, 1), 0.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(1, 0), 0.);
    EXPECT_EQ(m(1, 1), 40.);
    EXPECT_EQ(m(1, 2), 0.);
    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), 0.);
    EXPECT_EQ(m(2, 2), 45.);
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
    auto m = fe::matrix::stiffness<TET4>();
    EXPECT_EQ(m(0, 0), 1.);
    EXPECT_EQ(m(0, 1), -1.);
    EXPECT_EQ(m(0, 2), 0.);
    EXPECT_EQ(m(0, 3), 0.);

    EXPECT_EQ(m(1, 0), -1.);
    EXPECT_EQ(m(1, 1), 2.);
    EXPECT_EQ(m(1, 2), -1.);
    EXPECT_EQ(m(1, 3), 0.);

    EXPECT_EQ(m(2, 0), 0.);
    EXPECT_EQ(m(2, 1), -1.);
    EXPECT_EQ(m(2, 2), 2.);
    EXPECT_EQ(m(2, 3), -1.);

    EXPECT_EQ(m(3, 0), 0.);
    EXPECT_EQ(m(3, 1), 0.);
    EXPECT_EQ(m(3, 2), -1.);
    EXPECT_EQ(m(3, 3), 1.);
}

TEST(FEMatricesTest, stiffness_hex8)
{
    EXPECT_DEATH(fe::matrix::stiffness<HEX8>(), "Stiffness matrix in not implemented for HEX8");
}

TEST(FEMatricesTest, mass_surface_edge2)
{
    auto m = fe::matrix::mass_surface<EDGE2, 1>();
    EXPECT_EQ(m(0, 0), 1.);
}

TEST(FEMatricesTest, mass_surface_tri3)
{
    auto m = fe::matrix::mass_surface<TRI3, 2>();
    EXPECT_EQ(m(0, 0), 2.);
    EXPECT_EQ(m(0, 1), 1.);
    EXPECT_EQ(m(1, 0), 1.);
    EXPECT_EQ(m(1, 1), 2.);
}

TEST(FEMatricesTest, mass_surface_rz_edge2)
{
    DenseVector<Real, 1> rad_n({ 10. });
    auto m = fe::matrix::mass_surface_rz<EDGE2>(rad_n);
    EXPECT_EQ(m(0, 0), 10.);
}

TEST(FEMatricesTest, mass_surface_rz_tri3)
{
    DenseVector<Real, 2> rad_n({ 1., 2. });
    auto m = fe::matrix::mass_surface_rz<godzilla::TRI3>(rad_n);
    EXPECT_EQ(m(0, 0), 5.);
    EXPECT_EQ(m(0, 1), 3.);
    EXPECT_EQ(m(1, 0), 3.);
    EXPECT_EQ(m(1, 1), 7.);
}
