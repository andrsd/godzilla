#include "gmock/gmock.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/Parameters.h"

using namespace godzilla;
using namespace testing;

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

    Problem *
    get_prob()
    {
        return BoundaryCondition::get_problem();
    }
};

} // namespace

TEST_F(BoundaryConditionTest, api)
{
    this->mesh->create();
    this->prob->create();

    auto params = BoundaryCondition::parameters();
    params.set<App *>("_app", this->app)
        .set<DiscreteProblemInterface *>("_dpi", this->prob)
        .set<std::string>("_name", "obj")
        .set<std::vector<std::string>>("boundary", { "side1" });
    MockBoundaryCondition bc(params);
    bc.create();

    EXPECT_THAT(bc.get_boundary(), ElementsAre("side1"));
    EXPECT_THAT(bc.get_prob(), this->prob);
    EXPECT_THAT(bc.get_dimension(), 1_D);
}
