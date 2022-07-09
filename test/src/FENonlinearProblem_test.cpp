#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "FENonlinearProblem_test.h"
#include "CallStack.h"
#include "Factory.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "Parameters.h"
#include "LineMesh.h"
#include "UnstructuredMesh.h"
#include "InitialCondition.h"
#include "ConstantIC.h"
#include "BoundaryCondition.h"
#include "petsc.h"
#include "petscvec.h"

using namespace ::testing;
using namespace godzilla;

namespace {

/// Test IC with 2 components
class GTest2CompIC : public InitialCondition {
public:
    explicit GTest2CompIC(const Parameters & params) : InitialCondition(params) {}

    virtual PetscInt
    get_num_components() const
    {
        return 2;
    }

    virtual void
    evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[])
    {
        u[0] = 0.;
        u[1] = 10.;
    }
};

} // namespace

REGISTER_OBJECT(GTest2CompIC);

TEST_F(FENonlinearProblemTest, get_fepi_mesh)
{
    FEProblemInterface * fepi = dynamic_cast<FEProblemInterface *>(prob);
    UnstructuredMesh * unstr_mesh = dynamic_cast<UnstructuredMesh *>(mesh);
    EXPECT_EQ(fepi->get_mesh(), unstr_mesh);
}

TEST_F(FENonlinearProblemTest, fields)
{
    prob->add_fe(1, "vec", 3, 1);

    mesh->create();
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
}

TEST_F(FENonlinearProblemTest, add_duplicate_field_id)
{
    prob->add_fe(0, "first", 1, 1);
    EXPECT_DEATH(prob->add_fe(0, "second", 1, 1),
                 "\\[ERROR\\] Cannot add field 'second' with ID = 0. ID already exists.");
}

TEST_F(FENonlinearProblemTest, get_aux_fields)
{
    mesh->create();
    prob->add_aux_fe(0, "aux_one", 1, 1);
    prob->create();

    EXPECT_EQ(prob->get_aux_field_name(0), "aux_one");
    EXPECT_EQ(prob->get_aux_field_id("aux_one"), 0);
    EXPECT_EQ(prob->has_aux_field_by_id(0), true);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_one"), true);

    EXPECT_DEATH(prob->get_aux_field_name(1),
                 "\\[ERROR\\] Auxiliary field with ID = '1' does not exist.");
    EXPECT_DEATH(prob->get_aux_field_id("aux_two"),
                 "\\[ERROR\\] Auxiliary field 'aux_two' does not exist\\. Typo\\?");
    EXPECT_EQ(prob->has_aux_field_by_id(1), false);
    EXPECT_EQ(prob->has_aux_field_by_name("aux_two"), false);
}

TEST_F(FENonlinearProblemTest, add_duplicate_aux_field_id)
{
    prob->add_aux_fe(0, "first", 1, 1);
    EXPECT_DEATH(
        prob->add_aux_fe(0, "second", 1, 1),
        "\\[ERROR\\] Cannot add auxiliary field 'second' with ID = 0. ID is already taken.");
}

TEST_F(FENonlinearProblemTest, set_up_initial_guess)
{
    Parameters ic_pars = ConstantIC::parameters();
    ic_pars.set<const App *>("_app") = app;
    ic_pars.set<const DiscreteProblemInterface *>("_dpi") = prob;
    ic_pars.set<std::vector<PetscReal>>("value") = { 0 };
    ConstantIC ic(ic_pars);
    prob->add_initial_condition(&ic);

    mesh->create();
    prob->create();

    prob->set_up_initial_guess();

    Vec x = prob->get_solution_vector();
    PetscReal l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, zero_initial_guess)
{
    mesh->create();
    prob->create();
    prob->set_up_initial_guess();

    Vec x = prob->get_solution_vector();
    PetscReal l2_norm = 0;
    VecNorm(x, NORM_2, &l2_norm);
    EXPECT_DOUBLE_EQ(l2_norm, 0.);
}

TEST_F(FENonlinearProblemTest, solve)
{
    InitialCondition * ic = nullptr;
    {
        const std::string class_name = "ConstantIC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
        ic = this->app->build_object<InitialCondition>(class_name, "ic", params);
        prob->add_initial_condition(ic);
    }

    {
        const std::string class_name = "DirichletBC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const App *>("_app") = this->app;
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    prob->solve();

    bool conv = prob->converged();
    EXPECT_EQ(conv, true);

    const Vec x = prob->get_solution_vector();
    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.25);
}

TEST_F(FENonlinearProblemTest, solve_no_ic)
{
    {
        const std::string class_name = "DirichletBC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        auto bc = this->app->build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    mesh->create();
    prob->create();

    const Vec x = prob->get_solution_vector();
    PetscInt ni = 1;
    PetscInt ix[1] = { 0 };
    PetscScalar xx[1];
    VecGetValues(x, ni, ix, xx);
    EXPECT_DOUBLE_EQ(xx[0], 0.);
}

TEST_F(FENonlinearProblemTest, err_ic_comp_mismatch)
{
    testing::internal::CaptureStderr();

    {
        const std::string class_name = "GTest2CompIC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
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
        TestApp() : App("godzilla", MPI_COMM_WORLD) {}

        virtual void
        check_integrity()
        {
            if (this->log->get_num_entries() > 0)
                this->log->print();
        }
    } app;

    Mesh * mesh;
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<Mesh>(class_name, "mesh", params);
    }
    FENonlinearProblem * prob;
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        prob = app.build_object<FENonlinearProblem>(class_name, "prob", params);
    }
    {
        const std::string class_name = "ConstantIC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::string>("field") = "u";
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
        auto ic = app.build_object<InitialCondition>(class_name, "ic1", params);
        prob->add_initial_condition(ic);
    }
    const std::string class_name = "ConstantIC";
    Parameters * params = Factory::get_parameters(class_name);
    params->set<const DiscreteProblemInterface *>("_dpi") = prob;
    params->set<std::string>("field") = "u";
    params->set<std::vector<PetscReal>>("value") = { 0.2 };
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
        TestApp() : App("godzilla", MPI_COMM_WORLD) {}

        virtual void
        check_integrity()
        {
            if (this->log->get_num_entries() > 0)
                this->log->print();
        }
    } app;

    Mesh * mesh;
    FENonlinearProblem * prob;

    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<Mesh>(class_name, "mesh", params);
    }
    {
        const std::string class_name = "GTest2FieldsFENonlinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        prob = app.build_object<FENonlinearProblem>(class_name, "prob", params);
    }

    {
        const std::string class_name = "ConstantIC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::vector<PetscReal>>("value") = { 0.1 };
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
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        params->set<std::string>("boundary") = "asdf";
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

TEST_F(FENonlinearProblemTest, set_constant)
{
    std::vector<PetscReal> consts = { 5, 3, 1 };
    this->prob->set_constants(consts);

    auto & k = this->prob->getConstants();
    EXPECT_EQ(k[0], 5);
    EXPECT_EQ(k[1], 3);
    EXPECT_EQ(k[2], 1);
}

TEST_F(FENonlinearProblemTest, set_constant_2)
{
    this->mesh->create();
    this->prob->create();
    std::vector<PetscReal> consts = { 5, 3, 1 };
    this->prob->set_constants(consts);
    this->prob->set_up_constants();
    PetscDS ds = this->prob->getDS();
    PetscInt n_consts;
    const PetscReal * cs;
    PetscDSGetConstants(ds, &n_consts, &cs);
    EXPECT_EQ(n_consts, 3);
    EXPECT_EQ(cs[0], 5);
    EXPECT_EQ(cs[1], 3);
    EXPECT_EQ(cs[2], 1);
}
