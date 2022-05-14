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

        MOCK_METHOD(PetscInt, get_field_id, (), (const));
        MOCK_METHOD(PetscInt, get_num_components, (), (const));
        MOCK_METHOD((DMBoundaryConditionType), get_bc_type, (), (const));
        MOCK_METHOD(void,
                    evaluate,
                    (PetscInt, PetscReal, const PetscReal x[], PetscInt Nc, PetscScalar u[]),
                    ());
        MOCK_METHOD(std::vector<PetscInt>, get_components, (), (const));
        MOCK_METHOD(void, set_up_callback, ());
    };

    TestApp app;

    InputParameters params = BoundaryCondition::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "side1";
    MockBoundaryCondition bc(params);

    EXPECT_EQ(bc.get_boundary(), "side1");
}
