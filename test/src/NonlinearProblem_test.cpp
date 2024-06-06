#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/Factory.h"
#include "godzilla/Mesh.h"
#include "godzilla/LineMesh.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/Output.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

namespace {

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    explicit G1DTestNonlinearProblem(const Parameters & params);
    ~G1DTestNonlinearProblem() override;
    void create() override;
    void call_initial_guess();
    void solve() override;

protected:
    ErrorCode compute_residual(const Vector & x, Vector & f);
    ErrorCode compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);
    PetscSection s;
};

} // namespace

G1DTestNonlinearProblem::G1DTestNonlinearProblem(const Parameters & params) :
    NonlinearProblem(params),
    s(nullptr)
{
}

G1DTestNonlinearProblem::~G1DTestNonlinearProblem()
{
    PetscSectionDestroy(&this->s);
}

void
G1DTestNonlinearProblem::create()
{
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    auto dm = get_dm();
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, nullptr, nc, n_dofs, 0, nullptr, nullptr, nullptr, nullptr, &this->s);
    DMSetLocalSection(dm, this->s);
    NonlinearProblem::create();
    set_function(this, &G1DTestNonlinearProblem::compute_residual);
    set_jacobian(this, &G1DTestNonlinearProblem::compute_jacobian);
}

void
G1DTestNonlinearProblem::call_initial_guess()
{
    NonlinearProblem::set_up_initial_guess();
}

void
G1DTestNonlinearProblem::solve()
{
    NonlinearProblem::solve();
}

ErrorCode
G1DTestNonlinearProblem::compute_residual(const Vector & x, Vector & f)
{
    std::vector<Int> ix = { 0, 1 };
    std::vector<Scalar> y(2);
    x.get_values(ix, y);
    f.set_values({ 0, 1 }, { y[0] - 2, y[1] - 3 });
    f.assemble();
    return 0;
}

ErrorCode
G1DTestNonlinearProblem::compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp)
{
    J.set_value(0, 0, 1.);
    J.set_value(1, 1, 1.);
    J.assemble();
    return 0;
}

//

TEST(NonlinearProblemTest, initial_guess)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    G1DTestNonlinearProblem prob(prob_pars);
    prob.create();
    prob.call_initial_guess();

    auto x = prob.get_solution_vector();
    Real l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST(NonlinearProblemTest, solve)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    G1DTestNonlinearProblem prob(prob_pars);

    prob.create();
    prob.solve();

    bool conv = prob.converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    auto x = prob.get_solution_vector();
    Int ni = 2;
    Int ix[2] = { 0, 1 };
    Scalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);

    auto J = prob.get_jacobian();
    EXPECT_EQ(J(0, 0), 1.);
    EXPECT_EQ(J(0, 1), 0.);
    EXPECT_EQ(J(1, 0), 0.);
    EXPECT_EQ(J(1, 1), 1.);
}

TEST(NonlinearProblemTest, run)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(void, set_up_initial_guess, ());
        MOCK_METHOD(void, solve, ());
        MOCK_METHOD(void, on_initial, ());
        MOCK_METHOD(void, on_final, ());

        bool
        converged() override
        {
            return true;
        }
    };

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    MockNonlinearProblem prob(prob_pars);

    EXPECT_CALL(prob, set_up_initial_guess);
    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, on_initial);
    EXPECT_CALL(prob, on_final);
    prob.run();
}

TEST(NonlinearProblemTest, line_search_type)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}

        SNESolver
        get_snes()
        {
            return NonlinearProblem::get_snes();
        }
    };

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    std::vector<std::string> ls_type = { "basic", "l2", "cp", "nleqerr", "shell" };
    for (auto & lst : ls_type) {
        Parameters prob_pars = NonlinearProblem::parameters();
        prob_pars.set<App *>("_app") = &app;
        prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
        prob_pars.set<std::string>("line_search") = lst;
        MockNonlinearProblem prob(prob_pars);
        prob.create();

        SNES snes = prob.get_snes();
        SNESLineSearch ls;
        SNESGetLineSearch(snes, &ls);
        SNESLineSearchType tp;
        SNESLineSearchGetType(ls, &tp);
        EXPECT_STREQ(tp, lst.c_str());
    }
}

TEST(NonlinearProblemTest, invalid_line_search_type)
{
    testing::internal::CaptureStderr();

    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}
    };

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_pars.set<std::string>("line_search") = "asdf";
    MockNonlinearProblem prob(prob_pars);
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'line_search' parameter can be either 'bt', 'basic', 'l2', "
                                   "'cp', 'nleqerr' or 'shell'."));
}
