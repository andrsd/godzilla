#include "gmock/gmock.h"
#include "godzilla/Factory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/ExplicitFELinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/Parameters.h"
#include "godzilla/Output.h"
#include "TestApp.h"
#include "petsc.h"

using namespace godzilla;

namespace {

class TestExplicitFELinearProblem : public ExplicitFELinearProblem {
public:
    explicit TestExplicitFELinearProblem(const Parameters & params) :
        ExplicitFELinearProblem(params)
    {
    }

    void
    create() override
    {
        ExplicitFELinearProblem::create();
        create_mass_matrix();
    }

    void
    create_w_lumped_mass_matrix()
    {
        ExplicitFELinearProblem::create();
        create_mass_matrix_lumped();
    }

    void
    set_up_time_scheme() override
    {
        ExplicitFELinearProblem::set_up_time_scheme();
    }

    const Matrix &
    get_mass_matrix() const
    {
        return this->M;
    }

    const Vector &
    get_lumped_mass_matrix() const
    {
        return this->M_lumped_inv;
    }

protected:
    void
    set_up_fields() override
    {
        set_fe(0, "u", 1, 1);
    }

    void set_up_weak_form() override;
};

class TestF1 : public ResidualFunc {
public:
    explicit TestF1(TestExplicitFELinearProblem * prob) :
        ResidualFunc(prob),
        u(get_field_value("u")),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        Real visc = 1.;
        f[0] = -visc * this->u_x(0) + 0.5 * this->u(0) * this->u(0);
    }

protected:
    const FieldValue & u;
    const FieldGradient & u_x;
};

void
TestExplicitFELinearProblem::set_up_weak_form()
{
    add_residual_block(0, nullptr, new TestF1(this));
}

} // namespace

REGISTER_OBJECT(TestExplicitFELinearProblem);

TEST(ExplicitFELinearProblemTest, test_mass_matrix)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    mesh.create();
    prob.create();

    auto M = prob.get_mass_matrix();
    EXPECT_NEAR(M(0, 0), 0.1111111111111111, 1e-9);
    EXPECT_NEAR(M(0, 1), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(1, 0), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(1, 1), 0.2222222222222222, 1e-9);
    EXPECT_NEAR(M(1, 2), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(2, 1), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(2, 2), 0.2222222222222222, 1e-9);
    EXPECT_NEAR(M(2, 3), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(3, 2), 0.0555555555555555, 1e-9);
    EXPECT_NEAR(M(3, 3), 0.1111111111111111, 1e-9);
}

TEST(ExplicitFELinearProblemTest, test_lumped_mass_matrix)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    mesh.create();
    prob.create_w_lumped_mass_matrix();

    auto M = prob.get_lumped_mass_matrix();
    EXPECT_NEAR(M(0), 6., 1e-9);
    EXPECT_NEAR(M(1), 3., 1e-9);
    EXPECT_NEAR(M(2), 3., 1e-9);
    EXPECT_NEAR(M(3), 6., 1e-9);
}

TEST(ExplicitFELinearProblemTest, solve)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    Parameters bc_left_pars = DirichletBC::parameters();
    bc_left_pars.set<App *>("_app") = &app;
    bc_left_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_left_pars.set<std::vector<std::string>>("boundary") = { "left" };
    bc_left_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    Parameters bc_right_pars = DirichletBC::parameters();
    bc_right_pars.set<App *>("_app") = &app;
    bc_right_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_right_pars.set<std::vector<std::string>>("boundary") = { "right" };
    bc_right_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    mesh.create();
    prob.create();
    prob.check();

    prob.run();

    EXPECT_TRUE(prob.converged());
    EXPECT_DOUBLE_EQ(prob.get_time(), 1e-3);
    EXPECT_EQ(prob.get_step_num(), 1);

    auto sln = prob.get_solution_vector();
    auto x = sln.get_array_read();
    EXPECT_NEAR(x[0], 0.0118, 1e-15);
    EXPECT_NEAR(x[1], 0.0098, 1e-15);
    sln.restore_array_read(x);

    auto loc_sln = prob.get_solution_vector_local();
    auto lx = loc_sln.get_array_read();
    EXPECT_NEAR(lx[0], 1., 1e-15);
    EXPECT_NEAR(lx[1], 0.0118, 1e-15);
    EXPECT_NEAR(lx[2], 0.0098, 1e-15);
    EXPECT_NEAR(lx[3], 1., 1e-15);
    loc_sln.restore_array_read(lx);
}

TEST(ExplicitFELinearProblemTest, solve_w_lumped_mass_matrix)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 3;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    Parameters bc_left_pars = DirichletBC::parameters();
    bc_left_pars.set<App *>("_app") = &app;
    bc_left_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_left_pars.set<std::vector<std::string>>("boundary") = { "left" };
    bc_left_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    Parameters bc_right_pars = DirichletBC::parameters();
    bc_right_pars.set<App *>("_app") = &app;
    bc_right_pars.set<DiscreteProblemInterface *>("_dpi") = &prob;
    bc_right_pars.set<std::vector<std::string>>("boundary") = { "right" };
    bc_right_pars.set<std::vector<std::string>>("value") = { "1" };
    DirichletBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    mesh.create();
    prob.create_w_lumped_mass_matrix();
    prob.check();

    prob.run();

    EXPECT_TRUE(prob.converged());
    EXPECT_DOUBLE_EQ(prob.get_time(), 1e-3);
    EXPECT_EQ(prob.get_step_num(), 1);

    auto sln = prob.get_solution_vector();
    auto x = sln.get_array_read();
    EXPECT_NEAR(x[0], 0.0095, 1e-15);
    EXPECT_NEAR(x[1], 0.0085, 1e-15);
    sln.restore_array_read(x);

    auto loc_sln = prob.get_solution_vector_local();
    auto lx = loc_sln.get_array_read();
    EXPECT_NEAR(lx[0], 1., 1e-15);
    EXPECT_NEAR(lx[1], 0.0095, 1e-15);
    EXPECT_NEAR(lx[2], 0.0085, 1e-15);
    EXPECT_NEAR(lx[3], 1., 1e-15);
    loc_sln.restore_array_read(lx);
}

TEST(ExplicitFELinearProblemTest, set_schemes)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<Mesh *>("_mesh") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1e-3;
    prob_pars.set<Real>("dt") = 1e-3;
    prob_pars.set<std::string>("scheme") = "euler";
    TestExplicitFELinearProblem prob(prob_pars);

    mesh.create();
    prob.create();

    TS ts = prob.get_ts();
    TSType type;
    std::vector<std::string> schemes = { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" };
    std::vector<TSType> types = { TSEULER, TSSSP, TSSSP, TSRK, TSRK };
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
        Parameters * params = app.get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    {
        const std::string class_name = "TestExplicitFELinearProblem";
        Parameters * params = app.get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
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
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', "
                                   "'ssp-rk-3', 'rk-2' or 'heun'."));
}
