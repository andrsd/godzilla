#include "gmock/gmock.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/PCJacobi.h"
#include "TestApp.h"

using namespace godzilla;

TEST(KrylovSolver, tolerances)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);

    Real rel_tol = 1.4567e-5;
    Real abs_tol = 5.432e-25;
    Real div_tol = 1.234e4;
    Int max_its = 4321;
    ks.set_tolerances(rel_tol, abs_tol, div_tol, max_its);

    {
        Real rtol, atol, dtol;
        Int mits;
        ks.get_tolerances(&rtol, &atol, &dtol, &mits);
        EXPECT_DOUBLE_EQ(rtol, 1.4567e-5);
        EXPECT_DOUBLE_EQ(atol, 5.432e-25);
        EXPECT_DOUBLE_EQ(dtol, 1.234e4);
        EXPECT_EQ(mits, 4321);
    }

    {
        auto [rtol, atol, dtol, mits] = ks.get_tolerances();
        EXPECT_DOUBLE_EQ(rtol, 1.4567e-5);
        EXPECT_DOUBLE_EQ(atol, 5.432e-25);
        EXPECT_DOUBLE_EQ(dtol, 1.234e4);
        EXPECT_EQ(mits, 4321);
    }

    {
        Real rtol;
        Int mits;
        std::tie(rtol, std::ignore, std::ignore, mits) = ks.get_tolerances();
        EXPECT_DOUBLE_EQ(rtol, 1.4567e-5);
        EXPECT_EQ(mits, 4321);
    }
}

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
}

TEST(KrylovSolver, get_pc)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    auto pc = ks.get_pc();
    EXPECT_TRUE(pc != nullptr);
}

TEST(KrylovSolver, operator_ksp)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    KSP ksp = (KSP) ks;
    EXPECT_TRUE(ksp != nullptr);
}

TEST(KrylovSolver, set_monitor)
{
    class TestSolver {
    public:
        void
        monitor(Int it, Real rnorm)
        {
            this->norms.push_back(rnorm);
        }

        std::vector<Real> norms;
    };

    TestSolver solver;

    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    ks.monitor_set(ref(solver), &TestSolver::monitor);

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

    EXPECT_THAT(solver.norms,
                testing::ElementsAre(testing::DoubleEq(5.), testing::DoubleNear(1.1e-15, 1e-14)));
}

TEST(KrylovSolver, set_opers_rhs)
{
    class TestSystem {
    public:
        void
        compute_rhs(Vector & b)
        {
            b.set_value(0, 6);
            b.set_value(1, 12);
        }

        void
        compute_operators(Matrix & A, Matrix & B)
        {
            A.set_value(0, 0, 2);
            A.set_value(1, 1, 3);
            A.assemble();
        }
    };

    TestApp app;
    auto comm = app.get_comm();

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<Ref<godzilla::App>>("app", ref(app));
    mesh_pars.set<Real>("xmin", 0);
    mesh_pars.set<Real>("xmax", 1);
    mesh_pars.set<Int>("nx", 1);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto dm = mesh->get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    auto s = Section::create(dm, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
    DMSetLocalSection(dm, s);

    TestSystem sys;

    KrylovSolver ks;
    ks.create(comm);
    ks.set_dm(dm);
    ks.set_compute_operators(ref(sys), &TestSystem::compute_operators);
    ks.set_compute_rhs(ref(sys), &TestSystem::compute_rhs);

    Vector b = Vector::create_seq(comm, 2);
    ks.solve(b);

    EXPECT_DOUBLE_EQ(b(0), 3);
    EXPECT_DOUBLE_EQ(b(1), 4);
}

TEST(KrylovSolver, set_pc_type)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    Preconditioner pc = ks.set_pc_type<PCJacobi>();
    EXPECT_EQ(pc.get_type(), PCJACOBI);
}

TEST(KrylovSolver, get_rhs)
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

    auto rhs = ks.get_rhs();
    EXPECT_DOUBLE_EQ(rhs(0), 6.);
    EXPECT_DOUBLE_EQ(rhs(1), 12.);
}

TEST(KrylovSolver, get_operator)
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

    auto A = ks.get_operator();
    EXPECT_DOUBLE_EQ(A(0, 0), 2.);
    EXPECT_DOUBLE_EQ(A(1, 1), 3.);
}

TEST(KrylovSolver, get_operators)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);

    Matrix m = Matrix::create_seq_aij(comm, 2, 2, 1);
    m.set_value(0, 0, 2);
    m.set_value(1, 1, 3);
    m.assemble();

    Matrix p = Matrix::create_seq_aij(comm, 2, 2, 1);
    p.set_value(0, 0, 1);
    p.set_value(1, 1, 1.1);
    p.assemble();

    ks.set_operators(m, p);

    Vector b = Vector::create_seq(comm, 2);
    b.set_value(0, 6);
    b.set_value(1, 12);
    b.set_up();

    Vector x = b.duplicate();
    ks.solve(b, x);

    auto [A, P] = ks.get_operators();
    EXPECT_DOUBLE_EQ(A(0, 0), 2.);
    EXPECT_DOUBLE_EQ(A(1, 1), 3.);

    EXPECT_DOUBLE_EQ(P(0, 0), 1.);
    EXPECT_DOUBLE_EQ(P(1, 1), 1.1);
}

TEST(KrylovSolver, view)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    ks.set_type(KSPCG);

    testing::internal::CaptureStdout();
    ks.view();
    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, testing::HasSubstr("type: cg"));
}
