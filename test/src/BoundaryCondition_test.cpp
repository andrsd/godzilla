#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "BoundaryCondition.h"
#include "InputParameters.h"

using namespace godzilla;

TEST(BoundaryConditionTest, api)
{
    class MockBoundaryCondition : public BoundaryCondition {
    public:
        MockBoundaryCondition(const InputParameters & params) : BoundaryCondition(params) {}

        MOCK_METHOD((BoundaryConditionType), get_bc_type, (), (const));
        MOCK_METHOD(void, evaluate, (uint, Real, Real, Real, Real, Real), ());
    };

    TestApp app;

    InputParameters params = BoundaryCondition::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<std::string>("boundary") = "side1";
    MockBoundaryCondition bc(params);

    EXPECT_EQ(bc.get_boundary_name(), "side1");
}
