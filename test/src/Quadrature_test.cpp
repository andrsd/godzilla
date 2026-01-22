#include "gmock/gmock.h"
#include "godzilla/Quadrature.h"

using namespace godzilla;

TEST(QuadratureTest, gauss_tensor)
{
    auto q = Quadrature::create_gauss_tensor(2_D, 1, 2, -1, 1);
    EXPECT_EQ(q.get_dim(), 2);
    EXPECT_EQ(q.get_num_components(), 1);
    EXPECT_EQ(q.get_order(), 3);
}

TEST(QuadratureTest, dup)
{
    Quadrature q;
    q.create(MPI_COMM_WORLD);

    Quadrature r;
    q.duplicate(r);

    EXPECT_TRUE(q.equal(r));

    auto s = q.duplicate();
    EXPECT_TRUE(q.equal(s));
}

TEST(QuadratureTest, shallow)
{
    PetscQuadrature pq;
    PetscQuadratureCreate(MPI_COMM_WORLD, &pq);

    Quadrature q(pq);

    PetscBool eq;
    PetscQuadratureEqual(pq, (PetscQuadrature) q, &eq);
    EXPECT_TRUE(eq == PETSC_TRUE);
}

TEST(QuadratureTest, simplex)
{
    auto q = Quadrature::create_simplex(2_D, 3);
    EXPECT_EQ(q.get_dim(), 2);
    EXPECT_EQ(q.get_num_components(), 1);
    EXPECT_EQ(q.get_order(), 3);
}

TEST(QuadratureTest, stroud_conical)
{
    auto q = Quadrature::create_stroud_conical(2_D, 1, 2);
    EXPECT_EQ(q.get_dim(), 2);
    EXPECT_EQ(q.get_num_components(), 1);
    EXPECT_EQ(q.get_order(), 3);
}

TEST(QuadratureTest, tensor_quadrature)
{
    auto q1 = Quadrature::create_simplex(1_D, 3);
    auto q2 = Quadrature::create_simplex(2_D, 3);

    auto q = Quadrature::create_tensor_quadrature(q1, q2);
    EXPECT_EQ(q.get_dim(), 3);
    EXPECT_EQ(q.get_num_components(), 1);
    EXPECT_EQ(q.get_order(), 3);
}
