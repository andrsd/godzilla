#include "gmock/gmock.h"
#include "Factory.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "ExplicitFELinearProblem.h"
#include "ResidualFunc.h"
#include "Parameters.h"
#include "Output.h"
#include "TestApp.h"
#include "petsc.h"
#include "petscvec.h"

using namespace godzilla;

namespace {

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
        set_fe(0, "u", 1, 1);
    }
    virtual void set_up_weak_form() override;
};

class TestF1 : public ResidualFunc {
public:
    explicit TestF1(const TestExplicitFELinearProblem * prob) :
        ResidualFunc(prob),
        u(get_field_value("u")),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        Real visc = 1.;
        f[0] = -visc * this->u_x[0] + 0.5 * this->u[0] * this->u[0];
    }

protected:
    const FieldValue & u;
    const FieldGradient & u_x;
};

void
TestExplicitFELinearProblem::set_up_weak_form()
{
    set_residual_block(0, nullptr, new TestF1(this));
}

} // namespace

REGISTER_OBJECT(TestExplicitFELinearProblem);

TEST(ExplicitFELinearProblemTest, solve)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
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

    auto sln = prob.get_solution_vector();
    Int ni = 2;
    Int ix[2] = { 0, 1 };
    Scalar x[2];
    VecGetValues((Vec) sln, ni, ix, x);
    EXPECT_NEAR(x[0], 0.0118, 1e-15);
    EXPECT_NEAR(x[1], 0.0098, 1e-15);
}

TEST(ExplicitFELinearProblemTest, set_schemes)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
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
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    {
        const std::string class_name = "TestExplicitFELinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 20;
        params->set<Real>("dt") = 5;
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
