#include "gmock/gmock.h"
#include "godzilla/Factory.h"
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

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

private:
    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }

    void
    set_up_callbacks() override
    {
        set_compute([](Real time, const Real x[], Scalar u[]) { u[0] = 1.; });
    }
};

} // namespace

TEST(ExplicitFELinearProblemTest, test_mass_matrix)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.create();

    {
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
    {
        const TestExplicitFELinearProblem * cprob = &prob;
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    auto bc_left_pars = DirichletBC::parameters();
    bc_left_pars.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "left" });
    DirichletBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    auto bc_right_pars = DirichletBC::parameters();
    bc_right_pars.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "right" });
    DirichletBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    prob.create();

    prob.run();

    EXPECT_TRUE(prob.converged());
    EXPECT_DOUBLE_EQ(prob.get_time(), 1e-3);
    EXPECT_EQ(prob.get_step_num(), 1);

    auto sln = prob.get_solution_vector();
    auto x = sln.get_array_read();
    EXPECT_NEAR(x[0], 0.0118, 1e-15);
    EXPECT_NEAR(x[1], 0.0098, 1e-15);
    sln.restore_array_read(x);

    prob.compute_solution_vector_local();
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    auto bc_left_pars = DirichletBC::parameters();
    bc_left_pars.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "left" });
    DirichletBC bc_left(bc_left_pars);
    prob.add_boundary_condition(&bc_left);

    auto bc_right_pars = DirichletBC::parameters();
    bc_right_pars.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "right" });
    DirichletBC bc_right(bc_right_pars);
    prob.add_boundary_condition(&bc_right);

    prob.create_w_lumped_mass_matrix();

    prob.run();

    EXPECT_TRUE(prob.converged());
    EXPECT_DOUBLE_EQ(prob.get_time(), 1e-3);
    EXPECT_EQ(prob.get_step_num(), 1);

    auto sln = prob.get_solution_vector();
    auto x = sln.get_array_read();
    EXPECT_NEAR(x[0], 0.0095, 1e-15);
    EXPECT_NEAR(x[1], 0.0085, 1e-15);
    sln.restore_array_read(x);

    prob.compute_solution_vector_local();
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

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);

    prob.create();

    std::vector<std::string> schemes = { "euler", "ssp-rk-2", "ssp-rk-3", "rk-2", "heun" };
    std::vector<TSType> types = { TSEULER, TSSSP, TSSSP, TSRK, TSRK };
    for (std::size_t i = 0; i < schemes.size(); ++i) {
        prob.set_scheme(types[i]);
        EXPECT_EQ(prob.get_scheme(), types[i]);
    }
}

TEST(ExplicitFELinearProblemTest, wrong_scheme)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 20)
        .set<Real>("dt", 5)
        .set<std::string>("scheme", "asdf");
    TestExplicitFELinearProblem prob(prob_pars);

    prob.create();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', "
                                   "'ssp-rk-3', 'rk-2' or 'heun'."));
}

TEST(ExplicitFELinearProblemTest, allocate_mass_matrix)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.create();

    prob.allocate_mass_matrix();
    auto M = prob.get_mass_matrix();
    EXPECT_NE(M, nullptr);
}

TEST(ExplicitFELinearProblemTest, allocate_lumped_mass_matrix)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFELinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFELinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.create();

    prob.allocate_lumped_mass_matrix();
    {
        auto M = prob.get_lumped_mass_matrix();
        EXPECT_NE(M, nullptr);
    }
    {
        const auto & c_prob = prob;
        auto M = c_prob.get_lumped_mass_matrix();
        EXPECT_NE(M, nullptr);
    }
}
