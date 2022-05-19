#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "Factory.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "InputParameters.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "Output.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

registerObject(GTestImplicitFENonlinearProblem);

static void
f0_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f0[])
{
    f0[0] = u_t[0];
}

static void
f1_u(PetscInt dim,
     PetscInt Nf,
     PetscInt NfAux,
     const PetscInt uOff[],
     const PetscInt uOff_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt aOff[],
     const PetscInt aOff_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt numConstants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        f1[d] = u_x[d];
}

static void
g0_uu(PetscInt dim,
      PetscInt Nf,
      PetscInt NfAux,
      const PetscInt uOff[],
      const PetscInt uOff_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt aOff[],
      const PetscInt aOff_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_tShift,
      const PetscReal x[],
      PetscInt numConstants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    g3[0] = 1.0 * u_tShift;
}

static void
g3_uu(PetscInt dim,
      PetscInt Nf,
      PetscInt NfAux,
      const PetscInt uOff[],
      const PetscInt uOff_x[],
      const PetscScalar u[],
      const PetscScalar u_t[],
      const PetscScalar u_x[],
      const PetscInt aOff[],
      const PetscInt aOff_x[],
      const PetscScalar a[],
      const PetscScalar a_t[],
      const PetscScalar a_x[],
      PetscReal t,
      PetscReal u_tShift,
      const PetscReal x[],
      PetscInt numConstants,
      const PetscScalar constants[],
      PetscScalar g3[])
{
    PetscInt d;
    for (d = 0; d < dim; ++d)
        g3[d * dim + d] = 1.0;
}

TEST_F(ImplicitFENonlinearProblemTest, run)
{
    auto mesh = gMesh1d();
    auto prob = gProblem1d(mesh);
    this->app->problem = prob;

    {
        const std::string class_name = "ConstantIC";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const FEProblemInterface *>("_fepi") = prob;
        params->set<std::vector<PetscReal>>("value") = { 0 };
        auto ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    prob->run();

    const Vec x = prob->get_solution_vector();

    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);

    EXPECT_NEAR(xx[0], 0.5, 1e-7);
}

TEST_F(ImplicitFENonlinearProblemTest, output)
{
    class TestProblem : public GTestImplicitFENonlinearProblem {
    public:
        explicit TestProblem(const InputParameters & params) :
            GTestImplicitFENonlinearProblem(params)
        {
        }
        virtual void
        output()
        {
            ImplicitFENonlinearProblem::output();
        }
    };

    auto mesh = gMesh1d();

    InputParameters params = GTestImplicitFENonlinearProblem::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Mesh *>("_mesh") = mesh;
    params.set<PetscReal>("start_time") = 0.;
    params.set<PetscReal>("end_time") = 20;
    params.set<PetscReal>("dt") = 5;
    TestProblem prob(params);

    /// TOOD: make sure output() does not do anything
    prob.output();
}

TEST_F(ImplicitFENonlinearProblemTest, output_step)
{
    class MockImplicitFENonlinearProblem : public ImplicitFENonlinearProblem {
    public:
        explicit MockImplicitFENonlinearProblem(const InputParameters & params) :
            ImplicitFENonlinearProblem(params)
        {
        }

        virtual void
        output()
        {
            ImplicitFENonlinearProblem::output();
        }

        MOCK_METHOD(void, on_set_fields, ());
        MOCK_METHOD(void, on_set_weak_form, ());
    };

    class MockOutput : public Output {
    public:
        explicit MockOutput(const InputParameters & params) : Output(params) {}

        MOCK_METHOD(const std::string &, get_file_name, (), (const));
        MOCK_METHOD(void, set_file_name, ());
        MOCK_METHOD(void, set_sequence_file_name, (unsigned int stepi));
        MOCK_METHOD(void, output_step, (PetscInt stepi));
    };

    auto mesh = gMesh1d();
    mesh->create();

    InputParameters prob_pars = ImplicitFENonlinearProblem::valid_params();
    prob_pars.set<const App *>("_app") = this->app;
    prob_pars.set<const Mesh *>("_mesh") = mesh;
    MockImplicitFENonlinearProblem prob(prob_pars);

    InputParameters out_pars = Output::valid_params();
    out_pars.set<const App *>("_app") = this->app;
    out_pars.set<Problem *>("_problem") = &prob;
    MockOutput out(out_pars);

    prob.add_output(&out);

    EXPECT_CALL(out, output_step);

    prob.output();
}

// GTestImplicitFENonlinearProblem

GTestImplicitFENonlinearProblem::GTestImplicitFENonlinearProblem(const InputParameters & params) :
    ImplicitFENonlinearProblem(params),
    iu(0)
{
}

GTestImplicitFENonlinearProblem::~GTestImplicitFENonlinearProblem() {}

void
GTestImplicitFENonlinearProblem::on_set_fields()
{
    _F_;
    PetscInt order = 1;
    add_fe(this->iu, "u", 1, order);
}

void
GTestImplicitFENonlinearProblem::on_set_weak_form()
{
    set_residual_block(this->iu, f0_u, f1_u);
    set_jacobian_block(this->iu, this->iu, g0_uu, NULL, NULL, g3_uu);
}
