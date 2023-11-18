#include "gtest/gtest.h"
#include "godzilla/App.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/Output.h"

using namespace godzilla;

namespace {

class OutputTest : public FENonlinearProblemTest {
public:
};

class MockOutput : public Output {
public:
    explicit MockOutput(const Parameters & params) : Output(params) {}

    void
    output_step() override
    {
    }
};

} // namespace

TEST_F(OutputTest, exec_masks_1)
{
    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "none" };
    MockOutput out(pars);
    out.create();

    EXPECT_FALSE(out.should_output(ExecuteOn::FINAL));
    EXPECT_FALSE(out.should_output(ExecuteOn::INITIAL));
    EXPECT_FALSE(out.should_output(ExecuteOn::TIMESTEP));
}

TEST_F(OutputTest, exec_masks_2)
{
    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "final" };
    MockOutput out(pars);
    out.create();

    EXPECT_TRUE(out.get_exec_mask() & ExecuteOn::FINAL);
    EXPECT_TRUE(out.should_output(ExecuteOn::FINAL));
    EXPECT_FALSE(out.should_output(ExecuteOn::INITIAL));
    EXPECT_FALSE(out.should_output(ExecuteOn::TIMESTEP));
}

TEST_F(OutputTest, exec_masks_3)
{
    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "final", "initial", "timestep" };
    MockOutput out(pars);
    out.create();

    EXPECT_TRUE(out.get_exec_mask() & ExecuteOn::FINAL);
    EXPECT_TRUE(out.get_exec_mask() & ExecuteOn::INITIAL);
    EXPECT_TRUE(out.get_exec_mask() & ExecuteOn::TIMESTEP);

    EXPECT_TRUE(out.should_output(ExecuteOn::FINAL));
    EXPECT_TRUE(out.should_output(ExecuteOn::INITIAL));
    EXPECT_TRUE(out.should_output(ExecuteOn::TIMESTEP));
}

TEST_F(OutputTest, empty_on)
{
    testing::internal::CaptureStderr();

    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = {};
    MockOutput out(pars);

    out.create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'on' parameter can be either 'none' or a combination of "
                                   "'initial', 'timestep' and/or 'final'."));
}

TEST_F(OutputTest, none_plus_mask)
{
    testing::internal::CaptureStderr();

    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "none", "final", "timestep" };
    MockOutput out(pars);

    out.create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'none' execution flag can be used only by itself."));
}

TEST_F(OutputTest, interval_with_no_timestep_output)
{
    testing::internal::CaptureStderr();

    Parameters pars = Output::parameters();
    pars.set<App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "initial", "final" };
    pars.set<Int>("interval") = 10;
    MockOutput out(pars);

    out.create();
    out.check();

    app->check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        testing::HasSubstr("Parameter 'interval' was specified, but 'on' is missing 'timestep'."));
}
