#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "BoundaryCondition.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;
using namespace testing;

namespace {

class BoundaryConditionTest : public FENonlinearProblemTest {
public:
};

class MockBoundaryCondition : public BoundaryCondition {
public:
    explicit MockBoundaryCondition(const Parameters & params) : BoundaryCondition(params) {}

    MOCK_METHOD((DMBoundaryConditionType), get_bc_type, (), (const));
    MOCK_METHOD(void, evaluate, (Int, Real, const Real x[], Int Nc, Scalar u[]), ());
    MOCK_METHOD(const std::vector<Int> &, get_components, (), (const));
    virtual void
    set_up() override
    {
    }
};

} // namespace

TEST_F(BoundaryConditionTest, api)
{
    Parameters params = BoundaryCondition::parameters();
    params.set<const App *>("_app") = this->app;
    params.set<DiscreteProblemInterface *>("_dpi") = this->prob;
    params.set<std::string>("_name") = "obj";
    params.set<std::vector<std::string>>("boundary") = { "side1" };
    MockBoundaryCondition bc(params);

    EXPECT_THAT(bc.get_boundary(), ElementsAre("side1"));
}
