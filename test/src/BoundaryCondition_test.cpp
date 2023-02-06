#include "gmock/gmock.h"
#include "LineMesh.h"
#include "FENonlinearProblem_test.h"
#include "BoundaryCondition.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;

namespace {

class BoundaryConditionTest : public FENonlinearProblemTest {
public:
};

class BoundaryCondition2FieldTest : public FENonlinear2FieldProblemTest {
public:
};

class MockBoundaryCondition : public BoundaryCondition {
public:
    explicit MockBoundaryCondition(const Parameters & params) : BoundaryCondition(params) {}

    MOCK_METHOD((DMBoundaryConditionType), get_bc_type, (), (const));
    MOCK_METHOD(void, evaluate, (Int, PetscReal, const PetscReal x[], Int Nc, PetscScalar u[]), ());
    MOCK_METHOD(const std::vector<Int> &, get_components, (), (const));
    virtual void
    add_boundary() override
    {
    }
};

} // namespace

TEST_F(BoundaryConditionTest, api)
{
    Parameters params = BoundaryCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "side1";
    MockBoundaryCondition bc(params);

    EXPECT_EQ(bc.get_boundary(), "side1");
    EXPECT_EQ(bc.get_field_id(), -1);
}

TEST_F(BoundaryCondition2FieldTest, test)
{
    Parameters params = BoundaryCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "left";
    params.set<std::string>("field") = "u";
    MockBoundaryCondition bc(params);

    this->prob->add_boundary_condition(&bc);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_EQ(bc.get_field_id(), 0);
}

TEST_F(BoundaryCondition2FieldTest, no_field_param)
{
    testing::internal::CaptureStderr();

    Parameters params = BoundaryCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "left";
    MockBoundaryCondition bc(params);

    this->prob->add_boundary_condition(&bc);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr(
            "Use the 'field' parameter to assign this boundary condition to an existing field"));
}

TEST_F(BoundaryCondition2FieldTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    Parameters params = BoundaryCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<const DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "left";
    params.set<std::string>("field") = "asdf";
    MockBoundaryCondition bc(params);

    this->prob->add_boundary_condition(&bc);

    this->mesh->create();
    this->prob->create();

    this->app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("Field 'asdf' does not exists. Typo?"));
}
