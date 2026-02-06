#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/Parameters.h"

using namespace godzilla;

namespace {

class BoundaryConditionTest : public FENonlinearProblemTest {
public:
};

class MockBoundaryCondition : public BoundaryCondition {
public:
    explicit MockBoundaryCondition(const Parameters & pars) : BoundaryCondition(pars) {}

    MOCK_METHOD((DMBoundaryConditionType), get_bc_type, (), (const));
    MOCK_METHOD(void, evaluate, (Int, Real, const Real x[], Int Nc, Scalar u[]), ());
    MOCK_METHOD(const std::vector<Int> &, get_components, (), (const));

    void
    set_up() override
    {
    }
};

} // namespace

TEST_F(BoundaryConditionTest, api)
{
    auto prob = this->app->get_problem<GTestFENonlinearProblem>();

    prob->create();

    auto params = BoundaryCondition::parameters();
    params.set<Ref<App>>("app", ref(*this->app))
        .set<Ref<DiscreteProblemInterface>>("_dpi", prob)
        .set<String>("name", "obj")
        .set<std::vector<String>>("boundary", { "side1" });
    MockBoundaryCondition bc(params);
    bc.create();

    EXPECT_THAT(bc.get_boundary(), testing::ElementsAre("side1"));
    EXPECT_THAT(bc.get_dimension(), 1_D);
}
