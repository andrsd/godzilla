#include "gmock/gmock.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

TEST(KrylovSolver, solve)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);

    Matrix m = Matrix::create_seq_aij(comm, 2, 2, 1);
    m.set_value(0, 0, 2);
    m.set_value(1, 1, 3);
    m.assemble();
    ks.set_operators(m, m);

    Vector b = Vector::create_seq(comm, 2);
    b.set_value(0, 6);
    b.set_value(1, 12);
    b.set_up();

    Vector x = b.duplicate();
    ks.solve(b, x);

    EXPECT_DOUBLE_EQ(x(0), 3);
    EXPECT_DOUBLE_EQ(x(1), 4);

    ks.destroy();
}

TEST(KrylovSolver, solve_b)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);

    Matrix m = Matrix::create_seq_aij(comm, 2, 2, 1);
    m.set_value(0, 0, 2);
    m.set_value(1, 1, 3);
    m.assemble();
    ks.set_operator(m);

    Vector b = Vector::create_seq(comm, 2);
    b.set_value(0, 6);
    b.set_value(1, 12);
    b.set_up();

    ks.solve(b);

    EXPECT_DOUBLE_EQ(b(0), 3);
    EXPECT_DOUBLE_EQ(b(1), 4);

    ks.destroy();
}

TEST(KrylovSolver, get_pc)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    auto pc = ks.get_pc();
    EXPECT_TRUE(pc != nullptr);
    ks.destroy();
}

TEST(KrylovSolver, operator_ksp)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    KSP ksp = (KSP) ks;
    EXPECT_TRUE(ksp != nullptr);
    ks.destroy();
}
