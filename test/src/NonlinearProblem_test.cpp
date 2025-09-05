#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/LineMesh.h"
#include "godzilla/NonlinearProblem.h"
#include "godzilla/RestartOutput.h"
#include "godzilla/RestartFile.h"
#include "petscvec.h"
#include "petscdmplex.h"

using namespace godzilla;

namespace {

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    explicit G1DTestNonlinearProblem(const Parameters & params);
    ~G1DTestNonlinearProblem() override;
    void create() override;
    void call_initial_guess();

protected:
    void set_up_callbacks() override;
    void compute_residual(const Vector & x, Vector & f);
    void compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp);

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
}

void
G1DTestNonlinearProblem::call_initial_guess()
{
    NonlinearProblem::set_up_initial_guess();
}

void
G1DTestNonlinearProblem::set_up_callbacks()
{
    set_function(this, &G1DTestNonlinearProblem::compute_residual);
    set_jacobian(this, &G1DTestNonlinearProblem::compute_jacobian);
}

void
G1DTestNonlinearProblem::compute_residual(const Vector & x, Vector & f)
{
    std::vector<Int> ix = { 0, 1 };
    std::vector<Scalar> y(2);
    x.get_values(ix, y);
    f.set_values(std::vector<Int>({ 0, 1 }), { y[0] - 2, y[1] - 3 });
    f.assemble();
}

void
G1DTestNonlinearProblem::compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp)
{
    J.set_value(0, 0, 1.);
    J.set_value(1, 1, 1.);
    J.assemble();
}

//

TEST(NonlinearProblemTest, initial_guess)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    G1DTestNonlinearProblem prob(prob_pars);

    prob.create();
    prob.run();

    bool conv = prob.converged();
    EXPECT_EQ(conv, true);

    auto x = prob.get_solution_vector();
    EXPECT_DOUBLE_EQ(x(0), 2.);
    EXPECT_DOUBLE_EQ(x(1), 3.);

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
        MOCK_METHOD(void, on_initial, ());

        void
        compute_residual(const Vector & x, Vector & f)
        {
            f.zero();
            this->compute_residual_called = true;
        }

        void
        compute_jacobian(const Vector & x, Matrix & J, Matrix & Jp)
        {
            this->compute_jacobian_called = true;
        }

        void
        set_up_callbacks()
        {
            set_function(this, &MockNonlinearProblem::compute_residual);
            set_jacobian(this, &MockNonlinearProblem::compute_jacobian);
        }

        bool compute_residual_called = false;
        bool compute_jacobian_called = false;
    };

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = NonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    MockNonlinearProblem prob(prob_pars);
    prob.create();

    EXPECT_CALL(prob, set_up_initial_guess).Times(1);
    EXPECT_CALL(prob, on_initial).Times(1);
    prob.run();
    EXPECT_TRUE(prob.compute_residual_called);
    EXPECT_FALSE(prob.compute_jacobian_called);
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    std::vector<std::string> ls_type = { "basic", "l2", "cp", "nleqerr", "shell" };
    for (auto & lst : ls_type) {
        auto prob_pars = NonlinearProblem::parameters();
        prob_pars.set<App *>("_app", &app);
        prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
        prob_pars.set<std::string>("line_search", lst);
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = NonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<std::string>("line_search", "asdf");
    MockNonlinearProblem prob(prob_pars);
    prob.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'line_search' parameter can be either 'bt', 'basic', 'l2', "
                                   "'cp', 'nleqerr' or 'shell'."));
}

TEST(NonlinearProblemTest, restart_file)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 1);
    LineMesh mesh(mesh_pars);
    mesh.create();

    auto prob_pars = G1DTestNonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    G1DTestNonlinearProblem prob(prob_pars);

    auto ro_pars = RestartOutput::parameters();
    ro_pars.set<App *>("_app", &app);
    ro_pars.set<Problem *>("_problem", &prob);
    ro_pars.set<std::string>("file", "nl");
    RestartOutput ro(ro_pars);
    prob.add_output(&ro);

    prob.create();
    prob.run();

    {
        RestartFile f("nl.restart.h5", FileAccess::READ);
        auto v = Vector::create_seq(app.get_comm(), 2);
        f.read<Vector>("/", "sln", v);
        EXPECT_NEAR(v(0), 2, 1e-10);
        EXPECT_NEAR(v(1), 3, 1e-10);
    }
}
