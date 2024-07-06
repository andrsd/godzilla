#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/SNESolver.h"

using namespace godzilla;

namespace {

ErrorCode
compute_f(SNES, Vec x, Vec f, void *)
{
    Vector X(x);
    Vector F(f);
    std::vector<Int> ix = { 0, 1 };
    std::vector<Scalar> y(2);
    X.get_values(ix, y);
    F.set_values({ 0, 1 }, { y[0] - 2, y[1] - 3 });
    F.assemble();
    return 0;
}

ErrorCode
compute_jacobian(SNES, Vec, Mat j, Mat, void *)
{
    Matrix J(j);
    J.set_value(0, 0, 1.);
    J.set_value(1, 1, 1.);
    J.assemble();
    return 0;
}

//

class TestNLProblem {
public:
    void
    compute_f(const Vector & x, Vector & f)
    {
        std::vector<Int> ix = { 0, 1 };
        std::vector<Scalar> y(2);
        x.get_values(ix, y);
        f.set_values({ 0, 1 }, { y[0] - 2, y[1] - 3 });
        f.assemble();
    }

    ErrorCode
    compute_jacobian(const Vector &, Matrix & J, Matrix &)
    {
        J.set_value(0, 0, 1.);
        J.set_value(1, 1, 1.);
        J.assemble();
        return 0;
    }
};

} // namespace

TEST(SNESolverTest, solver_cls)
{
    TestApp app;
    TestNLProblem prob;
    auto comm = app.get_comm();

    SNESolver snes;
    snes.create(comm);

    Vector r = Vector::create_seq(comm, 2);
    snes.set_function(r, &prob, &TestNLProblem::compute_f);

    Matrix J = Matrix::create_seq_aij(comm, 2, 2, 1);
    snes.set_jacobian(J, J, &prob, &TestNLProblem::compute_jacobian);

    Vector x = r.duplicate();
    snes.solve(x);

    std::vector<Real> vals(2);
    x.get_values({ 0, 1 }, vals);

    EXPECT_DOUBLE_EQ(vals[0], 2.);
    EXPECT_DOUBLE_EQ(vals[1], 3.);

    snes.destroy();
}

TEST(SNESolverTest, solver_fns)
{
    TestApp app;
    TestNLProblem prob;
    auto comm = app.get_comm();

    SNESolver snes;
    snes.create(comm);

    Vector r = Vector::create_seq(comm, 2);
    snes.set_function(r, &compute_f);

    Matrix J = Matrix::create_seq_aij(comm, 2, 2, 1);
    snes.set_jacobian(J, J, &compute_jacobian);

    Vector x = r.duplicate();
    snes.solve(x);

    std::vector<Real> vals(2);
    x.get_values({ 0, 1 }, vals);

    EXPECT_DOUBLE_EQ(vals[0], 2.);
    EXPECT_DOUBLE_EQ(vals[1], 3.);

    snes.destroy();
}
