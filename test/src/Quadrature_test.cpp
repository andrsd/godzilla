#include "gmock/gmock.h"
#include "Quadrature.h"

using namespace godzilla;

TEST(QuadratureTest, gauss_tensor)
{
    Quadrature q = Quadrature::create_gauss_tensor(2, 1, 2, -1, 1);
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

    q.destroy();
}

TEST(QuadratureTest, shallow)
{
    PetscQuadrature pq;
    PetscQuadratureCreate(MPI_COMM_WORLD, &pq);

    Quadrature q(pq);

    PetscBool eq;
    PetscQuadratureEqual(pq, (PetscQuadrature) q, &eq);
    EXPECT_TRUE(eq == PETSC_TRUE);

    PetscQuadratureDestroy(&pq);
}
