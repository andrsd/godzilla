#include "gmock/gmock.h"
#include "TestApp.h"
#include "ExceptionTestMacros.h"
#include "godzilla/PCHypre.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"

using namespace godzilla;
using namespace testing;

#ifdef PETSC_HAVE_HYPRE

TEST(PCHypre, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    PCHypre hypre(pc);
    hypre.inc_reference();
    EXPECT_EQ(pc.get_type(), PCHYPRE);
    pc.destroy();
}

TEST(PCHypre, boomeramg)
{
    testing::internal::CaptureStdout();

    TestApp app;
    PCHypre pc;
    pc.create(app.get_comm());
    pc.set_type(PCHypre::BOOMERAMG);
    EXPECT_EQ(pc.get_type(), PCHypre::BOOMERAMG);
    pc.view();
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("type: hypre"));
    EXPECT_THAT(o, HasSubstr("HYPRE BoomerAMG preconditioning"));
}

TEST(PCHypre, set_type)
{
    TestApp app;
    std::vector<PCHypre::Type> hypre_types = { PCHypre::PILUT,
                                               PCHypre::PARASAILS,
                                               PCHypre::BOOMERAMG,
                                               PCHypre::AMS,
                                               PCHypre::ADS };
    for (auto & t : hypre_types) {
        PCHypre pc;
        pc.create(app.get_comm());
        pc.set_type(t);
        EXPECT_EQ(pc.get_type(), t);
        pc.destroy();
    }
}

    #if PETSC_VERSION_GE(3, 18, 0)
TEST(PCHypre, ams_set_interior_nodes)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto interior = Vector::create_seq(comm, 3);
    interior.set_values({ 0, 1, 2 }, { 1, 0, 1 });
    pc.ams_set_interior_nodes(interior);
    pc.view();
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("compatible subspace projection frequency 0"));
}
    #endif

TEST(PCHypre, set_alpha_poisson_matrix)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto A = Matrix::create_seq_aij(comm, 2, 2, 1);
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);
    A.assemble();
    pc.set_alpha_poisson_matrix(A);
    pc.view();
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("vector Poisson solver (passed in by user)"));
}

TEST(PCHypre, set_beta_poisson_matrix)
{
    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto A = Matrix::create_seq_aij(comm, 2, 2, 1);
    A.set_value(0, 0, 1.);
    A.set_value(1, 1, 1.);
    A.assemble();
    pc.set_beta_poisson_matrix(A);
    pc.view();
    pc.destroy();

    auto o = testing::internal::GetCapturedStdout();
    EXPECT_THAT(o, HasSubstr("scalar Poisson solver (passed in by user)"));
}

TEST(PCHypre, set_discrete_curl)
{
    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto C = Matrix::create_seq_aij(comm, 2, 2, 1);
    C.set_value(0, 0, 1.);
    C.set_value(1, 1, 1.);
    C.assemble();
    pc.set_discrete_curl(C);
    pc.destroy();
}

TEST(PCHypre, set_discrete_gradient)
{
    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto G = Matrix::create_seq_aij(comm, 2, 2, 1);
    G.set_value(0, 0, 1.);
    G.set_value(1, 1, 1.);
    G.assemble();
    pc.set_discrete_gradient(G);
    pc.destroy();
}

TEST(PCHypre, set_edge_constant_vectors_2d)
{
    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto v0 = Vector::create_seq(comm, 2);
    v0.set_values(std::vector<Int>({ 0, 1 }), { 1, 0 });
    auto v1 = Vector::create_seq(comm, 2);
    v1.set_values(std::vector<Int>({ 0, 1 }), { 0, 1 });
    pc.set_edge_constant_vectors(v0, v1);
    pc.destroy();
}

TEST(PCHypre, set_edge_constant_vectors_3d)
{
    TestApp app;
    auto comm = app.get_comm();
    PCHypre pc;
    pc.create(comm);
    pc.set_type(PCHypre::AMS);
    auto v0 = Vector::create_seq(comm, 3);
    v0.set_values({ 0, 1, 2 }, { 1, 0, 0 });
    auto v1 = Vector::create_seq(comm, 3);
    v1.set_values({ 0, 1, 2 }, { 0, 1, 0 });
    auto v2 = Vector::create_seq(comm, 3);
    v2.set_values({ 0, 1, 2 }, { 0, 0, 1 });
    pc.set_edge_constant_vectors(v0, v1, v2);
    pc.destroy();
}

#else

TEST(PCHypre, ctor_pc)
{
    TestApp app;
    Preconditioner pc;
    pc.create(app.get_comm());
    EXPECT_THROW_MSG({ PCHypre(pc); }, "PETSc was not built with HYPRE.");
}

#endif
