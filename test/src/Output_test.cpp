#include "gtest/gtest.h"
#include "App.h"
#include "FENonlinearProblem_test.h"
#include "Output.h"

using namespace godzilla;

namespace {

class OutputTest : public FENonlinearProblemTest {
public:
};

class MockOutput : public Output {
public:
    explicit MockOutput(const Parameters & params) : Output(params) {}
    virtual void
    output_step()
    {
    }

    unsigned int
    get_exec_mask() const
    {
        return this->on_mask;
    }
};

} // namespace

TEST_F(OutputTest, exec_masks_1)
{
    Parameters pars = Output::parameters();
    pars.set<const App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "none" };
    MockOutput out(pars);
    out.create();

    EXPECT_FALSE(out.should_output(Output::ON_FINAL));
    EXPECT_FALSE(out.should_output(Output::ON_INITIAL));
    EXPECT_FALSE(out.should_output(Output::ON_TIMESTEP));
}

TEST_F(OutputTest, exec_masks_2)
{
    Parameters pars = Output::parameters();
    pars.set<const App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "final" };
    MockOutput out(pars);
    out.create();

    EXPECT_EQ(out.get_exec_mask(), Output::ON_FINAL);
    EXPECT_TRUE(out.should_output(Output::ON_FINAL));
    EXPECT_FALSE(out.should_output(Output::ON_INITIAL));
    EXPECT_FALSE(out.should_output(Output::ON_TIMESTEP));
}

TEST_F(OutputTest, exec_masks_3)
{
    Parameters pars = Output::parameters();
    pars.set<const App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "final", "initial", "timestep" };
    MockOutput out(pars);
    out.create();

    EXPECT_EQ(out.get_exec_mask() & Output::ON_FINAL, Output::ON_FINAL);
    EXPECT_EQ(out.get_exec_mask() & Output::ON_INITIAL, Output::ON_INITIAL);
    EXPECT_EQ(out.get_exec_mask() & Output::ON_TIMESTEP, Output::ON_TIMESTEP);

    EXPECT_TRUE(out.should_output(Output::ON_FINAL));
    EXPECT_TRUE(out.should_output(Output::ON_INITIAL));
    EXPECT_TRUE(out.should_output(Output::ON_TIMESTEP));
}

TEST_F(OutputTest, empty_on)
{
    testing::internal::CaptureStderr();

    Parameters pars = Output::parameters();
    pars.set<const App *>("_app") = app;
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
    pars.set<const App *>("_app") = app;
    pars.set<Problem *>("_problem") = prob;
    pars.set<std::vector<std::string>>("on") = { "none", "final", "timestep" };
    MockOutput out(pars);

    out.create();

    app->check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                testing::HasSubstr("The 'none' execution flag can be used only by itself."));
}
