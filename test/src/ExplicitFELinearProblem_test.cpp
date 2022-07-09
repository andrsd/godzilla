#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "Factory.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "ExplicitFELinearProblem.h"
#include "Parameters.h"
#include "Output.h"
#include "TestApp.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

namespace {

static void
f1_u(PetscInt dim,
     PetscInt nf,
     PetscInt nf_aux,
     const PetscInt u_off[],
     const PetscInt u_off_x[],
     const PetscScalar u[],
     const PetscScalar u_t[],
     const PetscScalar u_x[],
     const PetscInt a_off[],
     const PetscInt a_off_x[],
     const PetscScalar a[],
     const PetscScalar a_t[],
     const PetscScalar a_x[],
     PetscReal t,
     const PetscReal x[],
     PetscInt num_constants,
     const PetscScalar constants[],
     PetscScalar f1[])
{
    PetscReal visc = 1.;
    f1[0] = -visc * u_x[0] + 0.5 * u[0] * u[0];
}

class TestExplicitFELinearProblem : public ExplicitFELinearProblem {
public:
    explicit TestExplicitFELinearProblem(const Parameters & params) :
        ExplicitFELinearProblem(params)
    {
    }

    virtual void
    set_up_time_scheme() override
    {
        ExplicitFELinearProblem::set_up_time_scheme();
    }

protected:
    virtual void
    set_up_fields() override
    {
        add_fe(0, "u", 1, 1);
    }
    virtual void
    set_up_weak_form() override
    {
        set_residual_block(0, nullptr, f1_u);
    }
};

} // namespace

REGISTER_OBJECT(TestExplicitFELinearProblem);

TEST(ExplicitFELinearProblemTest, solve)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);
    app.problem = &prob;

    Parameters bc_left_pars = DirichletBC::parameters();
    bc_left_pars.set<const App *>("_app") = &app;
    bc_left_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_left_pars.set<std::string>("boundary") = "left";
    bc_left_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    Parameters bc_right_pars = DirichletBC::parameters();
    bc_right_pars.set<const App *>("_app") = &app;
    bc_right_pars.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_right_pars.set<std::string>("boundary") = "right";
    bc_right_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    mesh.create();
    prob.create();
    prob.check();

    prob.run();

    EXPECT_TRUE(prob.converged());

    Vec sln = prob.get_solution_vector();
    PetscInt ni = 2;
    PetscInt ix[2] = { 0, 1 };
    PetscScalar x[2];
    VecGetValues(sln, ni, ix, x);
    EXPECT_NEAR(x[0], 0.0118, 1e-15);
    EXPECT_NEAR(x[1], 0.0098, 1e-15);
}

TEST(ExplicitFELinearProblemTest, set_schemes)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<PetscReal>("start_time") = 0.;
    prob_pars.set<PetscReal>("end_time") = 1e-3;
    prob_pars.set<PetscReal>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TS ts = prob.get_ts();
    TSType type;
    std::vector<std::string> schemes = { "euler", "ssp", "rk" };
    std::vector<TSType> types = { TSEULER, TSSSP, TSRK };
    for (std::size_t i = 0; i < schemes.size(); i++) {
        prob_pars.set<std::string>("scheme") = schemes[i];
        prob.set_up_time_scheme();
        TSGetType(ts, &type);
        EXPECT_STREQ(type, types[i]);
    }
}

TEST(ExplicitFELinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    TestApp app;
    LineMesh * mesh;
    TestExplicitFELinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    {
        const std::string class_name = "TestExplicitFELinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 20;
        params->set<PetscReal>("dt") = 5;
        params->set<std::string>("scheme") = "asdf";
        prob = app.build_object<TestExplicitFELinearProblem>(class_name, "prob", params);
    }

    mesh->create();
    prob->create();
    prob->check();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp' or 'rk'."));
}
