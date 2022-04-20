#include "Common.h"
#include "Mesh.h"
#include "Edge.h"
#include "RefMap1D.h"
#include "QuadratureGauss1D.h"
#include "gmock/gmock.h"

using namespace godzilla;

TEST(RefMap1DTest, base)
{
    Vertex1D v1(0.);
    Vertex1D v2(1.);

    Mesh mesh;
    mesh.set_vertex(0, &v1);
    mesh.set_vertex(1, &v2);

    Edge edge(0, 1);
    mesh.set_element(0, &edge);

    RefMap1D ref_map(&mesh);
    ref_map.set_active_element(&edge);

    EXPECT_EQ(ref_map.get_ref_order(), 0);
    EXPECT_EQ(ref_map.get_inv_ref_order(), 0);

    QuadratureGauss1D quad;
    uint qorder = 2;
    uint np = quad.get_num_points(qorder);
    QPoint1D * qpts = quad.get_points(qorder);

    Real1x1 * rm = ref_map.get_ref_map(np, qpts);
    EXPECT_DOUBLE_EQ(rm[0][0][0], 0.5);
    EXPECT_DOUBLE_EQ(rm[1][0][0], 0.5);

    Real * jac = ref_map.get_jacobian(np, qpts, false);
    EXPECT_DOUBLE_EQ(jac[0], 0.5);
    EXPECT_DOUBLE_EQ(jac[1], 0.5);

    Real * jxw = ref_map.get_jacobian(np, qpts, true);
    EXPECT_DOUBLE_EQ(jxw[0], 0.5);
    EXPECT_DOUBLE_EQ(jxw[1], 0.5);

    Real1x1 * irm = ref_map.get_inv_ref_map(np, qpts);
    EXPECT_DOUBLE_EQ(irm[0][0][0], 2.);
    EXPECT_DOUBLE_EQ(irm[1][0][0], 2.);

    Real * x = ref_map.get_phys_x(np, qpts);
    EXPECT_DOUBLE_EQ(x[0], 0.21132486540518503);
    EXPECT_DOUBLE_EQ(x[1], 0.78867513459481497);
}
