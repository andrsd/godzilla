#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/Factory.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/DirichletBC.h"
#include "godzilla/BoundaryCondition.h"
#include "ExceptionTestMacros.h"
#include "petscvec.h"

using namespace ::testing;
using namespace godzilla;

namespace {

/// Test IC with 2 components
class GTest2CompIC : public InitialCondition {
public:
    explicit GTest2CompIC(const Parameters & pars) : InitialCondition(pars) {}

    Int
    get_num_components() const override
    {
        return 2;
    }

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = 0.;
        u[1] = 10.;
    }
};

} // namespace

TEST_F(FENonlinearProblemTest, fields)
{
    prob->set_field(FieldID(1), "vec", 3, Order(1));

    auto aux_fld1_idx = prob->add_aux_field("aux_fld1", 2, Order(1));

    prob->create();

    EXPECT_EQ(prob->get_field_name(FieldID(0)), "u");
    EXPECT_EQ(prob->get_field_id("u"), FieldID(0));
    EXPECT_EQ(prob->has_field_by_id(FieldID(0)), true);
    EXPECT_EQ(prob->has_field_by_name("u"), true);

    EXPECT_THROW_MSG(
        { auto n = prob->get_field_name(FieldID(65536)); },
        "Field with ID = '65536' does not exist.");
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto id = prob->get_field_id("nonexistent"); },
        "Field 'nonexistent' does not exist. Typo?");
    EXPECT_EQ(prob->has_field_by_id(FieldID(65536)), false);
    EXPECT_EQ(prob->has_field_by_name("nonexistent"), false);

    EXPECT_EQ(prob->get_field_order(FieldID(0)), 1);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto o = prob->get_field_order(FieldID(65536)); },
        "Field with ID = '65536' does not exist.");

    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto nc = prob->get_field_num_components(FieldID(65536)); },
        "Field with ID = '65536' does not exist.");

    EXPECT_EQ(prob->get_field_component_name(FieldID(0), 0).compare(""), 0);
    EXPECT_EQ(prob->get_field_component_name(FieldID(1), 0).compare("0"), 0);
    EXPECT_EQ(prob->get_field_component_name(FieldID(1), 1).compare("1"), 0);
    EXPECT_EQ(prob->get_field_component_name(FieldID(1), 2).compare("2"), 0);
    prob->set_field_component_name(FieldID(1), 0, "x");
    EXPECT_EQ(prob->get_field_component_name(FieldID(1), 0).compare("x"), 0);
    EXPECT_THROW_MSG(
        { auto n = prob->get_field_component_name(FieldID(65536), 0); },
        "Field with ID = '65536' does not exist.");
    EXPECT_THROW_MSG(prob->set_field_component_name(FieldID(0), 0, "x"),
                     "Unable to set component name for single-component field");
    EXPECT_THROW_MSG(prob->set_field_component_name(FieldID(65536), 0, "x"),
                     "Field with ID = '65536' does not exist.");

    auto fld2_idx = prob->add_field("fld2", 3, Order(1));
    EXPECT_EQ(fld2_idx, FieldID(2));

    EXPECT_EQ(prob->get_field_dof(4, FieldID(0)), 8);
    EXPECT_EQ(prob->get_field_dof(4, FieldID(1)), 9);

    EXPECT_EQ(aux_fld1_idx, FieldID(0));

    EXPECT_EQ(prob->get_aux_field_dof(3, FieldID(0)), 2);
    EXPECT_EQ(prob->get_aux_field_dof(4, FieldID(0)), 4);
}

TEST_F(FENonlinearProblemTest, add_duplicate_field_id)
{
    prob->set_field(FieldID(0), "first", 1, Order(1));
    EXPECT_THROW_MSG(prob->set_field(FieldID(0), "second", 1, Order(1)),
                     "Cannot add field 'second' with ID = 0. ID already exists.");
}

TEST_F(FENonlinearProblemTest, get_aux_fields)
{
    prob->set_aux_field(FieldID(0), "aux_one", 1, Order(1));
    prob->add_aux_field("aux_two", 2, Order(1));
    prob->create();

    EXPECT_EQ(prob->get_aux_field_name(FieldID(0)), "aux_one");
    EXPECT_EQ(prob->get_aux_field_num_components(FieldID(0)), 1);
    EXPECT_EQ(prob->get_aux_field_num_components(FieldID(1)), 2);
    EXPECT_EQ(prob->get_aux_field_id("aux_one"), FieldID(0));
    EXPECT_EQ(prob->has_aux_field_by_id(FieldID(0)), true);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_one"), true);
    EXPECT_EQ(prob->get_aux_field_order(FieldID(0)), 1);

    prob->set_aux_field_component_name(FieldID(1), 1, "Y");
    EXPECT_TRUE(prob->get_aux_field_component_name(FieldID(0), 0) == "");
    EXPECT_TRUE(prob->get_aux_field_component_name(FieldID(1), 0) == "0");
    EXPECT_TRUE(prob->get_aux_field_component_name(FieldID(1), 1) == "Y");
    EXPECT_THROW_MSG(
        { auto n = prob->get_aux_field_component_name(FieldID(2), 1); },
        "Auxiliary field with ID = '2' does not exist.");

    EXPECT_THROW_MSG(
        { auto n = prob->get_aux_field_name(FieldID(2)); },
        "Auxiliary field with ID = '2' does not exist.");
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto nc = prob->get_aux_field_num_components(FieldID(2)); },
        "Auxiliary field with ID = '2' does not exist.");
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto id = prob->get_aux_field_id("aux_none"); },
        "Auxiliary field 'aux_none' does not exist. Typo?");
    EXPECT_EQ(prob->has_aux_field_by_id(FieldID(2)), false);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_none"), false);
    EXPECT_THROW_MSG(
        { [[maybe_unused]] auto o = prob->get_aux_field_order(FieldID(2)); },
        "Auxiliary field with ID = '2' does not exist.");
    EXPECT_THROW_MSG(prob->set_aux_field_component_name(FieldID(0), 1, "C"),
                     "Unable to set component name for single-component field");
    EXPECT_THROW_MSG(prob->set_aux_field_component_name(FieldID(2), 1, "C"),
                     "Auxiliary field with ID = '2' does not exist.");
}

TEST_F(FENonlinearProblemTest, add_duplicate_aux_field_id)
{
    prob->set_aux_field(FieldID(0), "first", 1, Order(1));
    EXPECT_THROW_MSG(prob->set_aux_field(FieldID(0), "second", 1, Order(1)),
                     "Cannot add auxiliary field 'second' with ID = 0. ID is already taken.");
}

TEST_F(FENonlinearProblemTest, set_up_initial_guess)
{
    auto ic_pars = ConstantInitialCondition::parameters();
    ic_pars.set<App *>("_app", app);
    ic_pars.set<DiscreteProblemInterface *>("_dpi", prob);
    ic_pars.set<std::vector<Real>>("value", { 0 });
    ConstantInitialCondition ic(ic_pars);
    prob->add_initial_condition(&ic);

    prob->create();

    prob->set_up_initial_guess();

    auto x = prob->get_solution_vector();
    Real l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, zero_initial_guess)
{
    prob->create();
    prob->set_up_initial_guess();

    auto x = prob->get_solution_vector();
    Real l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, solve)
{
    auto ic_pars = ConstantInitialCondition::parameters();
    ic_pars.set<App *>("_app", this->app);
    ic_pars.set<DiscreteProblemInterface *>("_dpi", this->prob);
    ic_pars.set<std::vector<Real>>("value", { 0.1 });
    ConstantInitialCondition ic(ic_pars);
    this->prob->add_initial_condition(&ic);

    auto params = DirichletBC::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", prob);
    params.set<std::vector<std::string>>("boundary", { "left", "right" });
    params.set<std::vector<std::string>>("value", { "x*x" });
    DirichletBC bc(params);
    this->prob->add_boundary_condition(&bc);
    this->prob->create();

    auto bcs = this->prob->get_boundary_conditions();
    EXPECT_EQ(bcs.size(), 1);
    EXPECT_EQ(bcs[0], &bc);

    auto ess_bcs = this->prob->get_essential_bcs();
    ASSERT_EQ(ess_bcs.size(), 1);
    EXPECT_EQ(bcs[0], &bc);

    this->prob->run();

    bool conv = this->prob->converged();
    EXPECT_EQ(conv, true);

    auto x = this->prob->get_solution_vector();
    EXPECT_DOUBLE_EQ(x(0), 0.25);
}

TEST_F(FENonlinearProblemTest, solve_no_ic)
{
    auto params = DirichletBC::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", prob);
    params.set<std::vector<std::string>>("boundary", { "marker" });
    params.set<std::vector<std::string>>("value", { "x*x" });
    DirichletBC bc(params);
    this->prob->add_boundary_condition(&bc);
    this->prob->create();

    auto x = prob->get_solution_vector();
    EXPECT_DOUBLE_EQ(x(0), 0.);
}

TEST_F(FENonlinearProblemTest, err_ic_comp_mismatch)
{
    testing::internal::CaptureStderr();

    auto params = GTest2CompIC::parameters();
    params.set<std::string>("_name", "ic");
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", prob);
    GTest2CompIC ic(params);
    this->prob->add_initial_condition(&ic);
    this->prob->create();
    EXPECT_FALSE(this->app->check_integrity());
    this->app->get_logger()->print();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Initial condition 'ic' operates on 2 components, but is set on a field "
                           "with 1 components."));
}

TEST(TwoFieldFENonlinearProblemTest, err_duplicate_ics)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_params = GTest2FieldsFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem prob(prob_params);

    auto ic1_params = ConstantInitialCondition::parameters();
    ic1_params.set<std::string>("_name", "ic1")
        .set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::string>("field", "u")
        .set<std::vector<Real>>("value", { 0.1 });
    ConstantInitialCondition ic1(ic1_params);
    prob.add_initial_condition(&ic1);

    auto ic2_params = ConstantInitialCondition::parameters();
    ic2_params.set<std::string>("_name", "ic2")
        .set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::string>("field", "u")
        .set<std::vector<Real>>("value", { 0.2 });
    ConstantInitialCondition ic2(ic2_params);
    prob.add_initial_condition(&ic2);

    prob.create();
    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Initial condition 'ic2' is being applied to a field that already has an "
                    "initial condition."));
}

TEST(TwoFieldFENonlinearProblemTest, err_not_enough_ics)
{
    testing::internal::CaptureStderr();

    class TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_params = GTest2FieldsFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem prob(prob_params);

    auto * ic_params = app.get_parameters("ConstantInitialCondition");
    ic_params->set<DiscreteProblemInterface *>("_dpi", &prob);
    ic_params->set<std::vector<Real>>("value", { 0.1 });
    ic_params->set<std::string>("field", "u");
    auto ic = app.build_object<InitialCondition>("ic1", ic_params);
    prob.add_initial_condition(ic);

    prob.create();
    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Provided 2 field(s), but 1 initial condition(s)."));
}

TEST_F(FENonlinearProblemTest, err_nonexisting_bc_bnd)
{
    testing::internal::CaptureStderr();

    auto params = DirichletBC::parameters();
    params.set<std::string>("_name", "bc1");
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", prob);
    params.set<std::vector<std::string>>("boundary", { "asdf" });
    params.set<std::vector<std::string>>("value", { "0.1" });
    DirichletBC bc(params);
    this->prob->add_boundary_condition(&bc);
    this->prob->create();
    EXPECT_FALSE(this->app->check_integrity());
    this->app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Boundary condition 'bc1' is set on boundary 'asdf' which does not exist "
                    "in the mesh."));
}

TEST(TwoFieldFENonlinearProblemTest, field_decomposition)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_params = GTest2FieldsFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem prob(prob_params);

    prob.create();

    auto fdecomp = prob.create_field_decomposition();
    ASSERT_EQ(fdecomp.get_num_fields(), 2);
    EXPECT_EQ(fdecomp.field_name[0], "Q1");
    EXPECT_EQ(fdecomp.field_name[1], "Q1");

    fdecomp.is[0].get_indices();
    auto idx0 = fdecomp.is[0].to_std_vector();
    EXPECT_THAT(idx0, ElementsAre(0, 2, 4));
    fdecomp.is[0].restore_indices();

    fdecomp.is[1].get_indices();
    auto idx1 = fdecomp.is[1].to_std_vector();
    EXPECT_THAT(idx1, ElementsAre(1, 3, 5));
    fdecomp.is[1].restore_indices();

    fdecomp.destroy();
    EXPECT_EQ(fdecomp.get_num_fields(), 0);
    EXPECT_EQ(fdecomp.field_name.size(), 0);
    EXPECT_EQ(fdecomp.is.size(), 0);
}

TEST_F(FENonlinearProblemTest, steady_state_output)
{
    auto params = DirichletBC::parameters();
    params.set<App *>("_app", this->app);
    params.set<DiscreteProblemInterface *>("_dpi", prob);
    params.set<std::vector<std::string>>("boundary", { "left", "right" });
    params.set<std::vector<std::string>>("value", { "x*x" });
    DirichletBC bc(params);
    this->prob->add_boundary_condition(&bc);

    this->prob->create();
    EXPECT_DOUBLE_EQ(this->prob->get_time(), 0.);
    this->prob->run();
    EXPECT_DOUBLE_EQ(this->prob->get_time(), 1.);
}
