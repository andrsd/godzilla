#include "gmock/gmock.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/ExplicitFELinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/Parameters.h"
#include "godzilla/Output.h"
#include "TestApp.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

class TestExplicitFELinearProblem : public ExplicitFELinearProblem {
public:
    explicit TestExplicitFELinearProblem(const Parameters & pars) : ExplicitFELinearProblem(pars) {}

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
    allocate_mass_matrix()
    {
        ExplicitProblemInterface::allocate_mass_matrix();
    }

    void
    allocate_lumped_mass_matrix()
    {
        ExplicitProblemInterface::allocate_lumped_mass_matrix();
    }

protected:
    void
    set_up_fields() override
    {
        set_field(FieldID(0), "u", 1, Order(1));
    }

    void set_up_weak_form() override;
    void set_up_time_scheme() override;
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
    add_residual_block(FieldID(0), nullptr, new TestF1(this));
}

void
TestExplicitFELinearProblem::set_up_time_scheme()
{
    set_scheme(TSEULER);
}

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = 1.;
    }
};

} // namespace

TEST(ExplicitFELinearProblemTest, test_mass_matrix)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    prob->create();

    {
        auto M = prob->get_mass_matrix();
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
    {
        const TestExplicitFELinearProblem * cprob = prob;
        auto M = cprob->get_mass_matrix();
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
}

TEST(ExplicitFELinearProblemTest, test_lumped_mass_matrix)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    prob->create_w_lumped_mass_matrix();

    auto M = prob->get_lumped_mass_matrix();
    EXPECT_NEAR(M(0), 6., 1e-9);
    EXPECT_NEAR(M(1), 3., 1e-9);
    EXPECT_NEAR(M(2), 3., 1e-9);
    EXPECT_NEAR(M(3), 6., 1e-9);
}

TEST(ExplicitFELinearProblemTest, solve)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    auto bc_left_pars = app.make_parameters<DirichletBC>();
    bc_left_pars.set<App *>("app", &app);
    bc_left_pars.set<std::vector<String>>("boundary", { "left" });
    prob->add_boundary_condition<DirichletBC>(bc_left_pars);

    auto bc_right_pars = app.make_parameters<DirichletBC>();
    bc_right_pars.set<App *>("app", &app);
    bc_right_pars.set<std::vector<String>>("boundary", { "right" });
    prob->add_boundary_condition<DirichletBC>(bc_right_pars);

    prob->create();

    prob->run();

    EXPECT_TRUE(prob->converged());
    EXPECT_DOUBLE_EQ(prob->get_time(), 1e-3);
    EXPECT_EQ(prob->get_step_num(), 1);

    auto sln = prob->get_solution_vector();
    auto x = sln.borrow_array_read();
    EXPECT_NEAR(x[0], 0.0118, 1e-15);
    EXPECT_NEAR(x[1], 0.0098, 1e-15);

    prob->compute_solution_vector_local();
    auto loc_sln = prob->get_solution_vector_local();
    auto lx = loc_sln.borrow_array_read();
    EXPECT_NEAR(lx[0], 1., 1e-15);
    EXPECT_NEAR(lx[1], 0.0118, 1e-15);
    EXPECT_NEAR(lx[2], 0.0098, 1e-15);
    EXPECT_NEAR(lx[3], 1., 1e-15);
}

TEST(ExplicitFELinearProblemTest, solve_w_lumped_mass_matrix)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    auto bc_left_pars = app.make_parameters<DirichletBC>();
    bc_left_pars.set<App *>("app", &app);
    bc_left_pars.set<std::vector<String>>("boundary", { "left" });
    prob->add_boundary_condition<DirichletBC>(bc_left_pars);

    auto bc_right_pars = app.make_parameters<DirichletBC>();
    bc_right_pars.set<App *>("app", &app);
    bc_right_pars.set<std::vector<String>>("boundary", { "right" });
    prob->add_boundary_condition<DirichletBC>(bc_right_pars);

    prob->create_w_lumped_mass_matrix();

    prob->run();

    EXPECT_TRUE(prob->converged());
    EXPECT_DOUBLE_EQ(prob->get_time(), 1e-3);
    EXPECT_EQ(prob->get_step_num(), 1);

    auto sln = prob->get_solution_vector();
    auto x = sln.borrow_array_read();
    EXPECT_NEAR(x[0], 0.0095, 1e-15);
    EXPECT_NEAR(x[1], 0.0085, 1e-15);

    prob->compute_solution_vector_local();
    auto loc_sln = prob->get_solution_vector_local();
    auto lx = loc_sln.borrow_array_read();
    EXPECT_NEAR(lx[0], 1., 1e-15);
    EXPECT_NEAR(lx[1], 0.0095, 1e-15);
    EXPECT_NEAR(lx[2], 0.0085, 1e-15);
    EXPECT_NEAR(lx[3], 1., 1e-15);
}

TEST(ExplicitFELinearProblemTest, allocate_mass_matrix)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    prob->create();

    prob->allocate_mass_matrix();
    auto M = prob->get_mass_matrix();
    EXPECT_NE(M, nullptr);
}

TEST(ExplicitFELinearProblemTest, allocate_lumped_mass_matrix)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<TestExplicitFELinearProblem>();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3);
    auto prob = app.make_problem<TestExplicitFELinearProblem>(prob_pars);

    prob->create();

    prob->allocate_lumped_mass_matrix();
    {
        auto M = prob->get_lumped_mass_matrix();
        EXPECT_NE(M, nullptr);
    }
    {
        const auto c_prob = prob;
        auto M = c_prob->get_lumped_mass_matrix();
        EXPECT_NE(M, nullptr);
    }
}
