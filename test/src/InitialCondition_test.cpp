#include "gmock/gmock.h"
#include "LineMesh.h"
#include "FENonlinearProblem_test.h"
#include "InitialCondition.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

class InitialConditionTest : public FENonlinearProblemTest {
public:
};

class InitialCondition2FieldTest : public FENonlinear2FieldProblemTest {
public:
};

class MockInitialCondition : public InitialCondition {
public:
    explicit MockInitialCondition(const InputParameters & params) : InitialCondition(params) {}

    PetscInt
    get_num_components() const
    {
        return 1.;
    }
    MOCK_METHOD(void,
                evaluate,
                (PetscInt, PetscReal, const PetscReal x[], PetscInt Nc, PetscScalar u[]),
                ());
};

TEST_F(InitialConditionTest, api)
{
    InputParameters params = InitialCondition::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const FEProblemInterface *>("_fepi") = this->prob;
    params.set<std::string>("_name") = "obj";
    MockInitialCondition ic(params);

    EXPECT_EQ(ic.get_field_id(), -1);

    PetscReal x[1] = { 0. };
    PetscReal u[1] = { 0. };
    EXPECT_CALL(ic, evaluate);
    PetscFunc * fn = ic.get_function();
    EXPECT_EQ((*fn)(1, 0., x, 1, u, &ic), 0);
}

TEST_F(InitialConditionTest, test)
{
    InputParameters params = InitialCondition::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const FEProblemInterface *>("_fepi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("field") = "u";
    MockInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_EQ(ic.get_field_id(), 0);
}

TEST_F(InitialCondition2FieldTest, no_field_param)
{
    testing::internal::CaptureStderr();

    InputParameters params = InitialCondition::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const FEProblemInterface *>("_fepi") = this->prob;
    params.set<std::string>("_name") = "obj";
    MockInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr(
            "Use the 'field' parameter to assign this initial condition to an existing field"));
}

TEST_F(InitialCondition2FieldTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    InputParameters params = InitialCondition::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const FEProblemInterface *>("_fepi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("field") = "asdf";
    MockInitialCondition ic(params);

    this->prob->add_initial_condition(&ic);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Field 'asdf' does not exists. Typo?"));
}
