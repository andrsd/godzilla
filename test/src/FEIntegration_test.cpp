#include "gmock/gmock.h"
#include "FEIntegration.h"

using namespace godzilla;

TEST(FEIntegrationTest, edge2)
{
    EXPECT_EQ((fe::integration_coeff<EDGE2, 1>()), 1. / 2.);
    EXPECT_EQ((fe::integration_coeff<EDGE2, 1, 1>()), 1. / 6.);
}

TEST(FEIntegrationTest, tri3)
{
    EXPECT_EQ((fe::integration_coeff<TRI3, 1>()), 1. / 3.);
    EXPECT_EQ((fe::integration_coeff<TRI3, 1, 1>()), 1. / 12.);
}

TEST(FEIntegrationTest, quad4)
{
    EXPECT_DEATH((fe::integration_coeff<QUAD4, 1>()),
                 "Integration coefficient for 'QUAD4' is not implemented");
    EXPECT_DEATH((fe::integration_coeff<QUAD4, 1, 1>()),
                 "Integration coefficient for 'QUAD4' is not implemented");
}

TEST(FEIntegrationTest, tet4)
{
    EXPECT_EQ((fe::integration_coeff<TET4, 1>()), 1. / 4.);
    EXPECT_EQ((fe::integration_coeff<TET4, 1, 1>()), 1. / 20.);
}

TEST(FEIntegrationTest, hex8)
{
    EXPECT_DEATH((fe::integration_coeff<HEX8, 1>()),
                 "Integration coefficient for 'HEX8' is not implemented");
    EXPECT_DEATH((fe::integration_coeff<HEX8, 1, 1>()),
                 "Integration coefficient for 'HEX8' is not implemented");
}
