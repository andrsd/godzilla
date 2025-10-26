#include "gmock/gmock.h"
#include "godzilla/CallStack.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/NaturalRiemannBC.h"
#include "godzilla/ExplicitFVLinearProblem.h"
#include "godzilla/Parameters.h"
#include "TestApp.h"
#include "ExceptionTestMacros.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestBC : public NaturalRiemannBC {
public:
    explicit TestBC(const Parameters & pars) :
        NaturalRiemannBC(pars),
        inlet(pars.get<bool>("inlet"))
    {
    }

    void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override
    {
        if (this->inlet)
            xG[0] = 1.;
        else
            xG[0] = xI[0];
    }

protected:
    bool inlet;

public:
    static Parameters
    parameters()
    {
        Parameters params = NaturalRiemannBC::parameters();
        params.add_required_param<bool>("inlet", "inlet?");
        return params;
    }
};

//

class TestExplicitFVLinearProblem;

class TestExplicitFVLinearProblem : public ExplicitFVLinearProblem {
public:
    explicit TestExplicitFVLinearProblem(const Parameters & pars) : ExplicitFVLinearProblem(pars) {}

    void
    create() override
    {
        ExplicitFVLinearProblem::create();
        create_mass_matrix();
    }

    void
    create_w_lumped_mass_matrix()
    {
        ExplicitFVLinearProblem::create();
        create_mass_matrix_lumped();
    }

    void
    compute_flux(const Real x[],
                 const Real n[],
                 const Scalar u_l[],
                 const Scalar u_r[],
                 Scalar flux[])
    {
        CALL_STACK_MSG();
        Real wind[] = { 0.5 };
        Real wn = 0;
        wn += wind[0] * n[0];
        flux[0] = (wn > 0 ? u_l[0] : u_r[0]) * wn;
    }

protected:
    void
    set_up_fields() override
    {
        add_field(FieldID(0), "u", 1);

        add_aux_field("a0", 1, Order(0));
        add_aux_field("a1", 2, Order(0));
    }

    void
    set_up_weak_form() override
    {
        set_riemann_solver(FieldID(0), this, &TestExplicitFVLinearProblem::compute_flux);
    }
};

} // namespace

TEST(ExplicitFVLinearProblemTest, api)
{
    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, se we use 2D mesh in this test
    auto mesh_pars = RectangleMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);
#endif

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFVLinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.create();

    EXPECT_EQ(prob.get_num_fields(), 1);
    EXPECT_THAT(prob.get_field_names(), testing::ElementsAre(""));

    EXPECT_EQ(prob.get_field_name(FieldID(0)), "u");
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto & n = prob.get_field_name(FieldID(65536)); },
        "Field with ID = '65536' does not exist.");

    EXPECT_EQ(prob.get_field_num_components(FieldID(0)), 1);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto id = prob.get_field_num_components(FieldID(65536)); },
        "Field with ID = '65536' does not exist.");

    EXPECT_EQ(prob.get_field_id("u"), FieldID(0));
    EXPECT_EQ(prob.get_field_id("nonexistent"), FieldID(0));

    EXPECT_TRUE(prob.has_field_by_id(FieldID(0)));
    EXPECT_FALSE(prob.has_field_by_id(FieldID(65536)));

    EXPECT_TRUE(prob.has_field_by_name("u"));
    EXPECT_FALSE(prob.has_field_by_name("nonexistent"));

    EXPECT_EQ(prob.get_field_order(FieldID(0)), 0);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto o = prob.get_field_order(FieldID(65536)); },
        "Multiple-field problems are not implemented");

    EXPECT_EQ(prob.get_field_component_name(FieldID(0), 0).compare("u"), 0);
    EXPECT_THROW_MSG(
        { auto n = prob.get_field_component_name(FieldID(65536), 0); },
        "Multiple-field problems are not implemented");

    EXPECT_THROW_MSG(prob.set_field_component_name(FieldID(0), 0, "x"),
                     "Unable to set component name for single-component field");

    EXPECT_EQ(prob.get_num_aux_fields(), 2);
    EXPECT_THAT(prob.get_aux_field_names(), ElementsAre("a0", "a1"));
    EXPECT_TRUE(prob.get_aux_field_name(FieldID(0)) == "a0");
    EXPECT_TRUE(prob.get_aux_field_name(FieldID(1)) == "a1");
    EXPECT_THROW_MSG(
        { auto n = prob.get_aux_field_name(FieldID(99)); },
        "Auxiliary field with ID = '99' does not exist.");
    EXPECT_EQ(prob.get_aux_field_num_components(FieldID(0)), 1);
    EXPECT_EQ(prob.get_aux_field_num_components(FieldID(1)), 2);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto nc = prob.get_aux_field_num_components(FieldID(99)); },
        "Auxiliary field with ID = '99' does not exist.");
    EXPECT_EQ(prob.get_aux_field_id("a0"), FieldID(0));
    EXPECT_EQ(prob.get_aux_field_id("a1"), FieldID(1));
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto id = prob.get_aux_field_id("non-existent"); },
        "Auxiliary field 'non-existent' does not exist. Typo?");
    EXPECT_TRUE(prob.has_aux_field_by_id(FieldID(0)));
    EXPECT_TRUE(prob.has_aux_field_by_id(FieldID(1)));
    EXPECT_FALSE(prob.has_aux_field_by_id(FieldID(99)));
    EXPECT_TRUE(prob.has_aux_field_by_name("a0"));
    EXPECT_TRUE(prob.has_aux_field_by_name("a1"));
    EXPECT_FALSE(prob.has_aux_field_by_name("non-existent"));
    EXPECT_EQ(prob.get_aux_field_order(FieldID(0)), 0);
    EXPECT_EQ(prob.get_aux_field_order(FieldID(1)), 0);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto o = prob.get_aux_field_order(FieldID(99)); },
        "Auxiliary field with ID = '99' does not exist.");
    EXPECT_TRUE(prob.get_aux_field_component_name(FieldID(0), 1) == "");
    EXPECT_TRUE(prob.get_aux_field_component_name(FieldID(1), 0) == "0");
    EXPECT_THROW_MSG(
        { auto n = prob.get_aux_field_component_name(FieldID(99), 0); },
        "Auxiliary field with ID = '99' does not exist.");
    EXPECT_THROW_MSG(prob.set_aux_field_component_name(FieldID(0), 0, "C"),
                     "Unable to set component name for single-component field");
    prob.set_aux_field_component_name(FieldID(1), 1, "Y");
    EXPECT_TRUE(prob.get_aux_field_component_name(FieldID(1), 1) == "Y");
    EXPECT_THROW_MSG(prob.set_aux_field_component_name(FieldID(99), 0, "A"),
                     "Auxiliary field with ID = '99' does not exist.");
}

TEST(ExplicitFVLinearProblemTest, fields)
{
    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, se we use 2D mesh in this test
    auto mesh_pars = RectangleMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);
#endif

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFVLinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.add_field(FieldID(1), "vec", 3);
    EXPECT_EQ(prob.get_field_id("vec"), FieldID(0));

    EXPECT_THROW_MSG(prob.add_field(FieldID(1), "dup", 1),
                     "Cannot add field 'dup' with ID = 1. ID already exists.");
    prob.set_field_component_name(FieldID(1), 0, "x");
    prob.set_field_component_name(FieldID(1), 1, "y");
    prob.set_field_component_name(FieldID(1), 2, "z");
    EXPECT_THROW_MSG(prob.set_field_component_name(FieldID(65536), 0, "A"),
                     "Field with ID = '65536' does not exist.");

    prob.create();

    EXPECT_EQ(prob.get_num_fields(), 1);
    EXPECT_THAT(prob.get_field_names(), testing::ElementsAre(""));

    EXPECT_EQ(prob.get_field_component_name(FieldID(0), 1).compare("vec_x"), 0);
    EXPECT_EQ(prob.get_field_component_name(FieldID(0), 2).compare("vec_y"), 0);
    EXPECT_EQ(prob.get_field_component_name(FieldID(0), 3).compare("vec_z"), 0);

    EXPECT_EQ(prob.get_field_dof(1, FieldID(0)), 4);
}

TEST(ExplicitFVLinearProblemTest, test_mass_matrix)
{
    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, se we use 2D mesh in this test
    auto mesh_pars = RectangleMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 3);
    mesh_pars.set<Int>("ny", 1);
    mesh_pars.set<Real>("ymax", 3.);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);
#endif

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFVLinearProblem prob(prob_pars);
    app.set_problem(&prob);

    prob.create();

    auto M = prob.get_mass_matrix();
    EXPECT_NEAR(M(0, 0), 1., 1e-9);
    EXPECT_NEAR(M(0, 1), 0., 1e-9);
    EXPECT_NEAR(M(1, 0), 0., 1e-9);
    EXPECT_NEAR(M(1, 1), 1., 1e-9);
    EXPECT_NEAR(M(1, 2), 0., 1e-9);
    EXPECT_NEAR(M(2, 1), 0., 1e-9);
    EXPECT_NEAR(M(2, 2), 1., 1e-9);
}

TEST(ExplicitFVLinearProblemTest, solve)
{
    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, se we use 2D mesh in this test
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFVLinearProblem prob(prob_pars);
    app.set_problem(&prob);

    auto bc_left_pars = TestBC::parameters();
    bc_left_pars.set<App *>("app", &app)
        .set<std::vector<std::string>>("boundary", { "left" })
        .set<bool>("inlet", true);
    auto bc_left = prob.add_boundary_condition<TestBC>(bc_left_pars);

    auto bc_right_pars = TestBC::parameters();
    bc_right_pars.set<App *>("app", &app)
        .set<std::vector<std::string>>("boundary", { "right" })
        .set<bool>("inlet", false);
    auto bc_right = prob.add_boundary_condition<TestBC>(bc_right_pars);

    mesh.create();
    prob.create();

    prob.run();

    EXPECT_TRUE(prob.converged());
    EXPECT_DOUBLE_EQ(prob.get_time(), 1e-3);
    EXPECT_EQ(prob.get_step_num(), 1);

    auto sln = prob.get_solution_vector();
    auto x = sln.get_array_read();
    EXPECT_NEAR(x[0], 0.001, 1e-15);
    EXPECT_NEAR(x[1], 0., 1e-15);
    sln.restore_array_read(x);

    prob.compute_solution_vector_local();
    auto loc_sln = prob.get_solution_vector_local();
    auto lx = loc_sln.get_array_read();
    EXPECT_NEAR(lx[0], 0.001, 1e-15);
    EXPECT_NEAR(lx[1], 0., 1e-15);
    EXPECT_NEAR(lx[2], 0., 1e-15);
    EXPECT_NEAR(lx[3], 0., 1e-15);
    loc_sln.restore_array_read(lx);
#endif
}

TEST(ExplicitFVLinearProblemTest, set_schemes)
{
    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, so we use 2D mesh in this test
    auto mesh_pars = RectangleMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);
#endif

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "euler");
    TestExplicitFVLinearProblem prob(prob_pars);

    prob.create();

    std::vector<TransientProblemInterface::TimeScheme> schemes = {
        TransientProblemInterface::TimeScheme::EULER,
        TransientProblemInterface::TimeScheme::SSP_RK_2,
        TransientProblemInterface::TimeScheme::SSP_RK_3,
        TransientProblemInterface::TimeScheme::RK_2,
        TransientProblemInterface::TimeScheme::HEUN
    };
    std::vector<TSType> types = { TSEULER, TSSSP, TSSSP, TSRK, TSRK };
    for (std::size_t i = 0; i < schemes.size(); ++i) {
        prob.set_scheme(schemes[i]);
        EXPECT_EQ(prob.get_scheme(), types[i]);
    }
}

TEST(ExplicitFVLinearProblemTest, wrong_schemes)
{
    testing::internal::CaptureStderr();

    TestApp app;

#if PETSC_VERSION_GE(3, 21, 0)
    // PETSc 3.21.0+ has a bug in forming the mass matrix in 1D, se we use 2D mesh in this test
    auto mesh_pars = RectangleMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 1);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);
#else
    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);
#endif

    auto prob_pars = TestExplicitFVLinearProblem::parameters();
    prob_pars.set<App *>("app", &app)
        .set<Mesh *>("mesh", mesh.get())
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1e-3)
        .set<Real>("dt", 1e-3)
        .set<std::string>("scheme", "asdf");
    TestExplicitFVLinearProblem prob(prob_pars);

    prob.create();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'scheme' parameter can be either 'euler', 'ssp-rk-2', "
                                   "'ssp-rk-3', 'rk-2' or 'heun'."));
}
