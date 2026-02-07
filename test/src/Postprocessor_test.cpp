#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "godzilla/App.h"
#include "godzilla/Enums.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

class PostprocessorTest : public ImplicitFENonlinearProblemTest {
public:
};

class MockPostprocessor : public Postprocessor {
public:
    explicit MockPostprocessor(const Parameters & pars) : Postprocessor(pars) {}

    void
    compute() override
    {
    }

    std::vector<Real>
    get_value() override
    {
        return { 1., 2., 5. };
    }
};

} // namespace

TEST_F(PostprocessorTest, exec_masks_1)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*this->app));
    pars.set<Ref<Problem>>("_problem", prob);
    pars.set<ExecuteOnFlags>("on", ExecuteOn::NONE);
    auto pp = prob->add_postprocessor<MockPostprocessor>(pars);

    EXPECT_FALSE(pp->should_execute(ExecuteOn::FINAL));
    EXPECT_FALSE(pp->should_execute(ExecuteOn::INITIAL));
    EXPECT_FALSE(pp->should_execute(ExecuteOn::TIMESTEP));
}

TEST_F(PostprocessorTest, exec_masks_2)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*this->app));
    pars.set<Ref<Problem>>("_problem", prob);
    pars.set<ExecuteOnFlags>("on", ExecuteOn::FINAL);
    auto pp = prob->add_postprocessor<MockPostprocessor>(pars);

    EXPECT_TRUE(pp->execute_on() & ExecuteOn::FINAL);
    EXPECT_TRUE(pp->should_execute(ExecuteOn::FINAL));
    EXPECT_FALSE(pp->should_execute(ExecuteOn::INITIAL));
    EXPECT_FALSE(pp->should_execute(ExecuteOn::TIMESTEP));
}

TEST_F(PostprocessorTest, exec_masks_3)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*this->app));
    pars.set<Ref<Problem>>("_problem", prob);
    pars.set<ExecuteOnFlags>("on", ExecuteOn::FINAL | ExecuteOn::INITIAL | ExecuteOn::TIMESTEP);
    auto pp = prob->add_postprocessor<MockPostprocessor>(pars);

    EXPECT_TRUE(pp->execute_on() & ExecuteOn::FINAL);
    EXPECT_TRUE(pp->execute_on() & ExecuteOn::INITIAL);
    EXPECT_TRUE(pp->execute_on() & ExecuteOn::TIMESTEP);

    prob->set_time(0.);
    EXPECT_TRUE(pp->should_execute(ExecuteOn::INITIAL));
    prob->set_time(0.1);
    EXPECT_TRUE(pp->should_execute(ExecuteOn::TIMESTEP));
    EXPECT_FALSE(pp->should_execute(ExecuteOn::TIMESTEP));
    prob->set_time(0.2);
    EXPECT_TRUE(pp->should_execute(ExecuteOn::FINAL));
}

TEST_F(PostprocessorTest, empty_on)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*app));
    pars.set<Ref<Problem>>("_problem", prob);
    pars.set<ExecuteOnFlags>("on", 0);

    EXPECT_DEATH(MockPostprocessor pp(pars),
                 "The 'on' parameter can be either 'none' or a combination of 'initial', "
                 "'timestep' and/or 'final'.");
}

TEST_F(PostprocessorTest, none_plus_mask)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*this->app));
    pars.set<Ref<Problem>>("_problem", prob);
    pars.set<ExecuteOnFlags>("on", ExecuteOn::NONE | ExecuteOn::FINAL | ExecuteOn::TIMESTEP);

    EXPECT_DEATH(MockPostprocessor pp(pars),
                 "The 'none' execution flag can be used only by itself.");
}

TEST_F(PostprocessorTest, default_execute_on_mask)
{
    auto prob = this->app->get_problem<GTestImplicitFENonlinearProblem>();

    auto pars = Postprocessor::parameters();
    pars.set<Ref<App>>("app", ref(*this->app));
    pars.set<Ref<Problem>>("_problem", prob);
    auto pp = prob->add_postprocessor<MockPostprocessor>(pars);

    EXPECT_FALSE(pp->execute_on() & ExecuteOn::FINAL);
    EXPECT_TRUE(pp->execute_on() & ExecuteOn::INITIAL);
    EXPECT_TRUE(pp->execute_on() & ExecuteOn::TIMESTEP);
}
