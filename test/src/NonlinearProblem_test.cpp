#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "Factory.h"
#include "Mesh.h"
#include "NonlinearProblem.h"
#include "Output.h"
#include "petsc.h"
#include "petscvec.h"
#include "petscmat.h"

using namespace godzilla;

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    explicit G1DTestNonlinearProblem(const Parameters & params);
    virtual ~G1DTestNonlinearProblem();
    virtual void create() override;
    void call_initial_guess();

protected:
    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

    PetscSection s;
};

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
    PetscInt nc[1] = { 1 };
    PetscInt n_dofs[2] = { 1, 0 };
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
G1DTestNonlinearProblem::compute_residual_callback(Vec x, Vec f)
{
    PetscInt ni = 2;
    PetscInt ix[] = { 0, 1 };
    PetscScalar y[2];
    VecGetValues(x, ni, ix, y);

    VecSetValue(f, 0, y[0] - 2, INSERT_VALUES);
    VecSetValue(f, 1, y[1] - 3, INSERT_VALUES);

    VecAssemblyBegin(f);
    VecAssemblyEnd(f);

    return 0;
}

PetscErrorCode
G1DTestNonlinearProblem::compute_jacobian_callback(Vec x, Mat J, Mat Jp)
{
    MatSetValue(J, 0, 0, 1, INSERT_VALUES);
    MatSetValue(J, 1, 1, 1, INSERT_VALUES);

    MatAssemblyBegin(J, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(J, MAT_FINAL_ASSEMBLY);

    return 0;
}

// Test fixture

class NonlinearProblemTest : public GodzillaAppTest {
protected:
    Mesh *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    G1DTestNonlinearProblem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "G1DTestNonlinearProblem";
        Parameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<G1DTestNonlinearProblem>(class_name, "problem", params);
    }
};

TEST_F(NonlinearProblemTest, initial_guess)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    prob->call_initial_guess();

    const Vec x = prob->get_solution_vector();
    PetscReal l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(NonlinearProblemTest, solve)
{
    auto mesh = gMesh1d();
    mesh->create();
    auto prob = gProblem1d(mesh);
    prob->create();
    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    // extract the solution and make sure it is [2, 3]
    const Vec x = prob->get_solution_vector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar xx[2];
    VecGetValues(x, ni, ix, xx);

    EXPECT_DOUBLE_EQ(xx[0], 2.);
    EXPECT_DOUBLE_EQ(xx[1], 3.);
}

TEST_F(NonlinearProblemTest, run)
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
        MOCK_METHOD(PetscErrorCode, compute_residual_callback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, compute_jacobian_callback, (Vec x, Mat J, Mat Jp));
    };

    auto mesh = gMesh1d();
    mesh->create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = mesh;
    MockNonlinearProblem prob(prob_pars);

    EXPECT_CALL(prob, set_up_initial_guess);
    EXPECT_CALL(prob, solve);
    EXPECT_CALL(prob, on_initial);
    EXPECT_CALL(prob, on_final);
    prob.run();
}

TEST_F(NonlinearProblemTest, line_search_type)
{
    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(PetscErrorCode, compute_residual_callback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, compute_jacobian_callback, (Vec x, Mat J, Mat Jp));

        SNES
        getSNES()
        {
            return this->snes;
        }
    };

    auto mesh = gMesh1d();
    mesh->create();

    std::vector<std::string> ls_type = { "basic", "l2", "cp", "nleqerr", "shell" };
    for (auto & lst : ls_type) {
        Parameters prob_pars = NonlinearProblem::parameters();
        prob_pars.set<const App *>("_app") = this->app;
        prob_pars.set<const Mesh *>("_mesh") = mesh;
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

TEST_F(NonlinearProblemTest, invalid_line_search_type)
{
    testing::internal::CaptureStderr();

    class MockNonlinearProblem : public NonlinearProblem {
    public:
        explicit MockNonlinearProblem(const Parameters & params) : NonlinearProblem(params) {}

        MOCK_METHOD(PetscErrorCode, compute_residual_callback, (Vec x, Vec f));
        MOCK_METHOD(PetscErrorCode, compute_jacobian_callback, (Vec x, Mat J, Mat Jp));
    };

    auto mesh = gMesh1d();
    mesh->create();

    Parameters prob_pars = NonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = mesh;
    prob_pars.set<std::string>("line_search") = "asdf";
    MockNonlinearProblem prob(prob_pars);
    prob.create();

    prob.check();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'line_search' parameter can be either 'bt', 'basic', 'l2', "
                                   "'cp', 'nleqerr' or 'shell'."));
}
