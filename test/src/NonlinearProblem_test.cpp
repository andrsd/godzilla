#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "Factory.h"
#include "Mesh.h"
#include "LineMesh.h"
#include "NonlinearProblem.h"
#include "Output.h"
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"

using namespace godzilla;

namespace {

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    explicit G1DTestNonlinearProblem(const Parameters & params);
    virtual ~G1DTestNonlinearProblem();
    virtual void create() override;
    void call_initial_guess();

    PetscErrorCode compute_residual(const Vector & x, Vector & f) override;
    PetscErrorCode compute_jacobian(Vec x, Mat J, Mat Jp) override;

protected:
    PetscSection s;
};

} // namespace

REGISTER_OBJECT(G1DTestNonlinearProblem);

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
    DM dm = get_dm();
    Int nc[1] = { 1 };
    Int n_dofs[2] = { 1, 0 };
    DMSetNumFields(dm, 1);
    DMPlexCreateSection(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL, &this->s);
    DMSetLocalSection(dm, this->s);
    NonlinearProblem::create();
}

void
G1DTestNonlinearProblem::call_initial_guess()
{
    NonlinearProblem::set_up_initial_guess();
}

PetscErrorCode
G1DTestNonlinearProblem::compute_residual(const Vector & x, Vector & f)
{
    std::vector<Int> ix = { 0, 1 };
    std::vector<Scalar> y(2);
    x.get_values(ix, y);

    f.set_values({ 0, 1 }, { y[0] - 2, y[1] - 3 });

    f.assembly_begin();
    f.assembly_end();

    return 0;
}

PetscErrorCode
G1DTestNonlinearProblem::compute_jacobian(Vec x, Mat J, Mat Jp)
{
    MatSetValue(J, 0, 0, 1, INSERT_VALUES);
    MatSetValue(J, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(J, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(J, MAT_FINAL_ASSEMBLY);

    return 0;
}

//

TEST(NonlinearProblemTest, initial_guess)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    G1DTestNonlinearProblem prob(prob_pars);
    prob.create();
    prob.call_initial_guess();

    auto x = prob.get_solution_vector();
    Real l2_norm = 0;
    VecNorm((Vec) x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST(NonlinearProblemTest, solve)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
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
    VecGetValues((Vec) x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}

TEST(NonlinearProblemTest, compute_callbacks)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    NonlinearProblem prob(prob_pars);

    Vector x;
    Vector F;
    EXPECT_EQ(prob.compute_residual(x, F), 0);

    Mat J;
    EXPECT_EQ(prob.compute_jacobian((Vec) x, J, J), 0);
}

TEST(NonlinearProblemTest, run)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(void, set_up_initial_guess, ());
        MOCK_METHOD(void, solve, ());
        virtual bool
        converged()
        {
            return true;
        }
        MOCK_METHOD(void, on_initial, ());
        MOCK_METHOD(void, on_final, ());
    };

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
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

        SNES
        getSNES()
        {
            return this->snes;
        }
    };

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    std::vector<std::string> ls_type = { "basic", "l2", "cp", "nleqerr", "shell" };
    for (auto & lst : ls_type) {
        Parameters prob_pars = NonlinearProblem::parameters();
        prob_pars.set<const App *>("_app") = &app;
        prob_pars.set<const Mesh *>("_mesh") = &mesh;
        prob_pars.set<std::string>("line_search") = lst;
        MockNonlinearProblem prob(prob_pars);
        prob.create();

        SNES snes = prob.getSNES();
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
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 1;
    LineMesh mesh(mesh_pars);
    mesh.create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<std::string>("line_search") = "asdf";
    MockNonlinearProblem prob(prob_pars);
    prob.create();

    prob.check();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'line_search' parameter can be either 'bt', 'basic', 'l2', "
                                   "'cp', 'nleqerr' or 'shell'."));
}
