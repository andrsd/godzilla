#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "BoundaryCondition.h"
#include "InputParameters.h"
#include "petsc.h"

using namespace godzilla;

TEST(BoundaryConditionTest, api)
{
    class MockBoundaryCondition : public BoundaryCondition {
    public:
        explicit MockBoundaryCondition(const InputParameters & params) : BoundaryCondition(params)
        {
        }

        MOCK_METHOD(PetscInt, getFieldId, (), (const));
        MOCK_METHOD(PetscInt, getNumComponents, (), (const));
        MOCK_METHOD((DMBoundaryConditionType), getBcType, (), (const));
        MOCK_METHOD(void,
                    evaluate,
                    (PetscInt, PetscReal, const PetscReal x[], PetscInt Nc, PetscScalar u[]),
                    ());
        MOCK_METHOD(std::vector<PetscInt>, getComponents, (), (const));
        MOCK_METHOD(void, setUpCallback, ());
    };

    TestApp app;

    InputParameters params = BoundaryCondition::validParams();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "side1";
    MockBoundaryCondition bc(params);

    EXPECT_EQ(bc.getBoundary(), "side1");
}
