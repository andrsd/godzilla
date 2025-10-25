#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "godzilla/App.h"
#include "godzilla/Enums.h"
#include "godzilla/Output.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

class OutputTest : public ImplicitFENonlinearProblemTest {
public:
};

class MockOutput : public Output {
public:
    explicit MockOutput(const Parameters & pars) : Output(pars) {}

    void
    output_step() override
    {
    }
};

} // namespace

TEST_F(OutputTest, exec_masks_1)
{
    auto pars = Output::parameters();
    pars.set<App *>("_app", this->app);
    pars.set<Problem *>("_problem", this->prob);
    pars.set<ExecuteOn>("on", EXECUTE_ON_NONE);
    MockOutput out(pars);
    out.create();

    EXPECT_FALSE(out.should_output(EXECUTE_ON_FINAL));
    EXPECT_FALSE(out.should_output(EXECUTE_ON_INITIAL));
    EXPECT_FALSE(out.should_output(EXECUTE_ON_TIMESTEP));
}

TEST_F(OutputTest, exec_masks_2)
{
    auto pars = Output::parameters();
    pars.set<App *>("_app", this->app);
    pars.set<Problem *>("_problem", this->prob);
    pars.set<ExecuteOn>("on", EXECUTE_ON_FINAL);
    MockOutput out(pars);
    out.create();

    EXPECT_TRUE(out.execute_on() & EXECUTE_ON_FINAL);
    EXPECT_TRUE(out.should_output(EXECUTE_ON_FINAL));
    EXPECT_FALSE(out.should_output(EXECUTE_ON_INITIAL));
    EXPECT_FALSE(out.should_output(EXECUTE_ON_TIMESTEP));
}

TEST_F(OutputTest, exec_masks_3)
{
    auto pars = Output::parameters();
    pars.set<App *>("_app", this->app);
    pars.set<Problem *>("_problem", this->prob);
    pars.set<ExecuteOn>("on", EXECUTE_ON_FINAL | EXECUTE_ON_INITIAL | EXECUTE_ON_TIMESTEP);
    MockOutput out(pars);
    out.create();

    EXPECT_TRUE(out.execute_on() & EXECUTE_ON_FINAL);
    EXPECT_TRUE(out.execute_on() & EXECUTE_ON_INITIAL);
    EXPECT_TRUE(out.execute_on() & EXECUTE_ON_TIMESTEP);

    this->prob->set_time(0.);
    EXPECT_TRUE(out.should_output(EXECUTE_ON_INITIAL));
    this->prob->set_time(0.1);
    EXPECT_TRUE(out.should_output(EXECUTE_ON_TIMESTEP));
    EXPECT_FALSE(out.should_output(EXECUTE_ON_TIMESTEP));
    this->prob->set_time(0.2);
    EXPECT_TRUE(out.should_output(EXECUTE_ON_FINAL));
}

TEST_F(OutputTest, empty_on)
{
    testing::internal::CaptureStderr();

    auto pars = Output::parameters();
    pars.set<App *>("_app", app);
    pars.set<Problem *>("_problem", prob);
    pars.set<ExecuteOn>("on", 0);
    MockOutput out(pars);

    out.create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'on' parameter can be either 'none' or a combination of "
                                   "'initial', 'timestep' and/or 'final'."));
}

TEST_F(OutputTest, none_plus_mask)
{
    testing::internal::CaptureStderr();

    auto pars = Output::parameters();
    pars.set<App *>("_app", this->app);
    pars.set<Problem *>("_problem", this->prob);
    pars.set<ExecuteOn>("on", EXECUTE_ON_NONE | EXECUTE_ON_FINAL | EXECUTE_ON_TIMESTEP);
    MockOutput out(pars);

    out.create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'none' execution flag can be used only by itself."));
}

TEST_F(OutputTest, interval_with_no_timestep_output)
{
    testing::internal::CaptureStderr();

    auto pars = Output::parameters();
    pars.set<App *>("_app", app);
    pars.set<Problem *>("_problem", prob);
    pars.set<ExecuteOn>("on", EXECUTE_ON_INITIAL | EXECUTE_ON_FINAL);
    pars.set<Int>("interval", 10);
    MockOutput out(pars);

    out.create();

    EXPECT_FALSE(app->check_integrity());
    app->get_logger()->print();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Parameter 'interval' was specified, but 'on' is missing 'timestep'."));
}
