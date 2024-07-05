#include "gmock/gmock.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/Matrix.h"
#include "godzilla/Vector.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/PCJacobi.h"
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

TEST(KrylovSolver, set_monitor)
{
    class TestSolver {
    public:
        ErrorCode
        monitor(Int it, Real rnorm)
        {
            this->norms.push_back(rnorm);
            return 0;
        }

        std::vector<Real> norms;
    };

    TestSolver solver;

    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    ks.monitor_set(&solver, &TestSolver::monitor);

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

    EXPECT_THAT(solver.norms, ElementsAre(DoubleEq(5.), DoubleNear(1.1e-15, 1e-14)));

    ks.destroy();
}

TEST(KrylovSolver, set_opers_rhs)
{
    class TestSystem {
    public:
        ErrorCode
        compute_rhs(Vector & b)
        {
            b.set_value(0, 6);
            b.set_value(1, 12);
            return 0;
        }

        ErrorCode
        compute_operators(Matrix & A, Matrix & B)
        {
            A.set_value(0, 0, 2);
            A.set_value(1, 1, 3);
            A.assemble();
            return 0;
        }
    };

    TestApp app;
    auto comm = app.get_comm();

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    mesh_pars.set<Real>("xmin") = 0;
    mesh_pars.set<Real>("xmax") = 1;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto dm = m->get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    auto s = Section::create(dm, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
    DMSetLocalSection(dm, s);

    TestSystem sys;

    KrylovSolver ks;
    ks.create(comm);
    ks.set_dm(dm);
    ks.set_compute_operators(&sys, &TestSystem::compute_operators);
    ks.set_compute_rhs(&sys, &TestSystem::compute_rhs);

    Vector b = Vector::create_seq(comm, 2);
    ks.solve(b);

    EXPECT_DOUBLE_EQ(b(0), 3);
    EXPECT_DOUBLE_EQ(b(1), 4);

    ks.destroy();
}

TEST(KrylovSolver, set_opers_rhs_c_version)
{
    std::vector<Real> norms;

    class TestSystem {
    public:
        static ErrorCode
        compute_rhs(KSP ksp, Vec b, void * ctx)
        {
            VecSetValue(b, 0, 6, INSERT_VALUES);
            VecSetValue(b, 1, 12, INSERT_VALUES);
            return 0;
        }

        static ErrorCode
        compute_operators(KSP ksp, Mat A, Mat B, void * ctx)
        {
            MatSetValue(A, 0, 0, 2., INSERT_VALUES);
            MatSetValue(A, 1, 1, 3., INSERT_VALUES);
            MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
            MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
            return 0;
        }

        static ErrorCode
        monitor(KSP, Int it, Real rnorm, void * ctx)
        {
            auto norms = static_cast<std::vector<Real> *>(ctx);
            (*norms).push_back(rnorm);
            return 0;
        }
    };

    TestApp app;
    auto comm = app.get_comm();

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    mesh_pars.set<Real>("xmin") = 0;
    mesh_pars.set<Real>("xmax") = 1;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto dm = m->get_dm();
    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    auto s = Section::create(dm, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr);
    DMSetLocalSection(dm, s);

    TestSystem sys;

    KrylovSolver ks;
    ks.create(comm);
    ks.set_dm(dm);
    ks.set_compute_operators(TestSystem::compute_operators);
    ks.set_compute_rhs(&TestSystem::compute_rhs);
    ks.monitor_set(TestSystem::monitor, &norms);

    Vector b = Vector::create_seq(comm, 2);
    ks.solve(b);

    EXPECT_DOUBLE_EQ(b(0), 3);
    EXPECT_DOUBLE_EQ(b(1), 4);
    EXPECT_THAT(norms, ElementsAre(DoubleEq(5.), DoubleNear(1.1e-15, 1e-14)));

    ks.destroy();
}

TEST(KrylovSolver, ctor_ksp)
{
    TestApp app;
    auto comm = app.get_comm();

    KSP ksp;
    KSPCreate(comm, &ksp);

    KrylovSolver ks(ksp);
    EXPECT_EQ(static_cast<KSP>(ks), ksp);

    KSPDestroy(&ksp);
}

TEST(KrylovSolver, set_pc_type)
{
    TestApp app;
    auto comm = app.get_comm();
    KrylovSolver ks;
    ks.create(comm);
    Preconditioner pc = ks.set_pc_type<PCJacobi>();
    EXPECT_EQ(pc.get_type(), PCJACOBI);
    ks.destroy();
}
