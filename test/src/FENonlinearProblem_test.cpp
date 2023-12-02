#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/CallStack.h"
#include "godzilla/Factory.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/Parameters.h"
#include "godzilla/LineMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/ConstantInitialCondition.h"
#include "godzilla/BoundaryCondition.h"
#include "petscvec.h"

using namespace ::testing;
using namespace godzilla;

namespace {

/// Test IC with 2 components
class GTest2CompIC : public InitialCondition {
public:
    explicit GTest2CompIC(const Parameters & params) : InitialCondition(params) {}

    Int
    get_num_components() const override
    {
        return 2;
    }

    void
    evaluate(Int dim, Real time, const Real x[], Int Nc, Scalar u[]) override
    {
        u[0] = 0.;
        u[1] = 10.;
    }
};

} // namespace

REGISTER_OBJECT(GTest2CompIC);

TEST_F(FENonlinearProblemTest, fields)
{
    mesh->create();

    prob->set_fe(1, "vec", 3, 1);

    Int aux_fld1_idx = prob->add_aux_fe("aux_fld1", 2, 1);

    prob->create();

    EXPECT_EQ(prob->get_field_name(0), "u");
    EXPECT_EQ(prob->get_field_id("u"), 0);
    EXPECT_EQ(prob->has_field_by_id(0), true);
    EXPECT_EQ(prob->has_field_by_name("u"), true);

    EXPECT_DEATH(prob->get_field_name(65536),
                 "\\[ERROR\\] Field with ID = '65536' does not exist.");
    EXPECT_DEATH(prob->get_field_id("nonexistent"),
                 "\\[ERROR\\] Field 'nonexistent' does not exist\\. Typo\\?");
    EXPECT_EQ(prob->has_field_by_id(65536), false);
    EXPECT_EQ(prob->has_field_by_name("nonexistent"), false);

    EXPECT_EQ(prob->get_field_order(0), 1);
    EXPECT_DEATH(prob->get_field_order(65536),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");

    EXPECT_DEATH(prob->get_field_num_components(65536),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");

    EXPECT_EQ(prob->get_field_component_name(0, 0).compare(""), 0);
    EXPECT_EQ(prob->get_field_component_name(1, 0).compare("0"), 0);
    EXPECT_EQ(prob->get_field_component_name(1, 1).compare("1"), 0);
    EXPECT_EQ(prob->get_field_component_name(1, 2).compare("2"), 0);
    prob->set_field_component_name(1, 0, "x");
    EXPECT_EQ(prob->get_field_component_name(1, 0).compare("x"), 0);
    EXPECT_DEATH(prob->get_field_component_name(65536, 0),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");
    EXPECT_DEATH(prob->set_field_component_name(0, 0, "x"),
                 "\\[ERROR\\] Unable to set component name for single-component field");
    EXPECT_DEATH(prob->set_field_component_name(65536, 0, "x"),
                 "\\[ERROR\\] Field with ID = '65536' does not exist\\.");

    Int fld2_idx = prob->add_fe("fld2", 3, 1);
    EXPECT_EQ(fld2_idx, 2);

    EXPECT_EQ(prob->get_field_dof(4, 0), 8);
    EXPECT_EQ(prob->get_field_dof(4, 1), 9);

    EXPECT_EQ(aux_fld1_idx, 0);

    EXPECT_EQ(prob->get_aux_field_dof(3, 0), 2);
    EXPECT_EQ(prob->get_aux_field_dof(4, 0), 4);
}

TEST_F(FENonlinearProblemTest, add_duplicate_field_id)
{
    mesh->create();
    prob->set_fe(0, "first", 1, 1);
    EXPECT_DEATH(prob->set_fe(0, "second", 1, 1),
                 "\\[ERROR\\] Cannot add field 'second' with ID = 0. ID already exists.");
}

TEST_F(FENonlinearProblemTest, get_aux_fields)
{
    mesh->create();
    prob->set_aux_fe(0, "aux_one", 1, 1);
    prob->add_aux_fe("aux_two", 2, 1);
    prob->create();

    EXPECT_EQ(prob->get_aux_field_name(0), "aux_one");
    EXPECT_EQ(prob->get_aux_field_num_components(0), 1);
    EXPECT_EQ(prob->get_aux_field_num_components(1), 2);
    EXPECT_EQ(prob->get_aux_field_id("aux_one"), 0);
    EXPECT_EQ(prob->has_aux_field_by_id(0), true);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_one"), true);
    EXPECT_EQ(prob->get_aux_field_order(0), 1);

    prob->set_aux_field_component_name(1, 1, "Y");
    EXPECT_TRUE(prob->get_aux_field_component_name(0, 0) == "");
    EXPECT_TRUE(prob->get_aux_field_component_name(1, 0) == "0");
    EXPECT_TRUE(prob->get_aux_field_component_name(1, 1) == "Y");
    EXPECT_DEATH(prob->get_aux_field_component_name(2, 1),
                 "\\[ERROR\\] Auxiliary field with ID = '2' does not exist.");

    EXPECT_DEATH(prob->get_aux_field_name(2),
                 "\\[ERROR\\] Auxiliary field with ID = '2' does not exist.");
    EXPECT_DEATH(prob->get_aux_field_num_components(2),
                 "\\[ERROR\\] Auxiliary field with ID = '2' does not exist.");
    EXPECT_DEATH(prob->get_aux_field_id("aux_none"),
                 "\\[ERROR\\] Auxiliary field 'aux_none' does not exist\\. Typo\\?");
    EXPECT_EQ(prob->has_aux_field_by_id(2), false);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_none"), false);
    EXPECT_DEATH(prob->get_aux_field_order(2),
                 "\\[ERROR\\] Auxiliary field with ID = '2' does not exist.");
    EXPECT_DEATH(prob->set_aux_field_component_name(0, 1, "C"),
                 "\\[ERROR\\] Unable to set component name for single-component field");
    EXPECT_DEATH(prob->set_aux_field_component_name(2, 1, "C"),
                 "\\[ERROR\\] Auxiliary field with ID = '2' does not exist.");
}

TEST_F(FENonlinearProblemTest, add_duplicate_aux_field_id)
{
    mesh->create();
    prob->set_aux_fe(0, "first", 1, 1);
    EXPECT_DEATH(
        prob->set_aux_fe(0, "second", 1, 1),
        "\\[ERROR\\] Cannot add auxiliary field 'second' with ID = 0. ID is already taken.");
}

TEST_F(FENonlinearProblemTest, set_up_initial_guess)
{
    Parameters ic_pars = ConstantInitialCondition::parameters();
    ic_pars.set<App *>("_app") = app;
    ic_pars.set<DiscreteProblemInterface *>("_dpi") = prob;
    ic_pars.set<std::vector<Real>>("value") = { 0 };
    ConstantInitialCondition ic(ic_pars);
    prob->add_initial_condition(&ic);

    mesh->create();
    prob->create();

    prob->set_up_initial_guess();

    auto x = prob->get_solution_vector();
    Real l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, zero_initial_guess)
{
    mesh->create();
    prob->create();
    prob->set_up_initial_guess();

    auto x = prob->get_solution_vector();
    Real l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, solve)
{
    InitialCondition * ic = nullptr;
    {
        const std::string class_name = "ConstantInitialCondition";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<Real>>("value") = { 0.1 };
        ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<App *>("_app") = this->app;
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<std::string>>("boundary") = { "left", "right" };
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    auto bcs = prob->get_boundary_conditions();
    EXPECT_EQ(bcs.size(), 1);
    EXPECT_EQ(bcs[0]->get_type(), "DirichletBC");

    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    auto x = prob->get_solution_vector();
    Int ni = 1;
    Int ix[1] = { 0 };
    Scalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.25);
}

TEST_F(FENonlinearProblemTest, solve_no_ic)
{
    {
        const std::string class_name = "DirichletBC";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<std::string>>("boundary") = { "marker" };
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    auto x = prob->get_solution_vector();
    Int ni = 1;
    Int ix[1] = { 0 };
    Scalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.);
}

TEST_F(FENonlinearProblemTest, err_ic_comp_mismatch)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "GTest2CompIC";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        auto ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }
    mesh->create();
    prob->create();
    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Initial condition 'ic' operates on 2 components, but is set on a field "
                           "with 1 components."));
}

TEST(TwoFieldFENonlinearProblemTest, err_duplicate_ics)
{
    testing::internal::CaptureStderr();

    class TestApp : public App {
    public:
        TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla") {}
    } app;

    Mesh * mesh;
    {
        const std::string class_name = "LineMesh";
        Parameters * params = app.get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<Mesh>(class_name, "mesh", params);
    }
    FENonlinearProblem * prob;
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        Parameters * params = app.get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        prob = app.build_object<FENonlinearProblem>(class_name, "prob", params);
    }
    {
        const std::string class_name = "ConstantInitialCondition";
        Parameters * params = app.get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::string>("field") = "u";
        params->set<std::vector<Real>>("value") = { 0.1 };
        auto ic = app.build_object<InitialCondition>(class_name, "ic1", params);
        prob->add_initial_condition(ic);
    }
    const std::string class_name = "ConstantInitialCondition";
    Parameters * params = app.get_parameters(class_name);
    params->set<DiscreteProblemInterface *>("_dpi") = prob;
    params->set<std::string>("field") = "u";
    params->set<std::vector<Real>>("value") = { 0.2 };
    auto ic = app.build_object<InitialCondition>(class_name, "ic2", params);
    prob->add_initial_condition(ic);
    mesh->create();
    prob->create();
    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Initial condition 'ic2' is being applied to a field that already has an "
                    "initial condition."));
}

TEST(TwoFieldFENonlinearProblemTest, err_not_enough_ics)
{
    testing::internal::CaptureStderr();

    class TestApp : public App {
    public:
        TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla") {}
    } app;

    UnstructuredMesh * mesh;
    FENonlinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        Parameters * params = app.get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        Parameters * params = app.get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        prob = app.build_object<FENonlinearProblem>(class_name, "prob", params);
    }

    {
        const std::string class_name = "ConstantInitialCondition";
        Parameters * params = app.get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<Real>>("value") = { 0.1 };
        params->set<std::string>("field") = "u";
        auto ic = app.build_object<InitialCondition>(class_name, "ic1", params);
        prob->add_initial_condition(ic);
    }

    mesh->create();
    prob->create();
    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Provided 2 field(s), but 1 initial condition(s)."));
}

TEST_F(FENonlinearProblemTest, err_nonexisting_bc_bnd)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "DirichletBC";
        Parameters * params = this->app->get_parameters(class_name);
        params->set<DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<std::string>>("boundary") = { "asdf" };
        params->set<std::vector<std::string>>("value") = { "0.1" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc1", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();
    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr(
                    "Boundary condition 'bc1' is set on boundary 'asdf' which does not exist "
                    "in the mesh."));
}

TEST_F(FENonlinearProblemTest, natural_riemann_bcs_not_supported)
{
    Label label;
    EXPECT_DEATH(prob->add_boundary_natural_riemann("", "", -1, {}, nullptr, nullptr, nullptr),
                 "\\[ERROR\\] Natural Riemann BCs are not supported for FE problems");
}
