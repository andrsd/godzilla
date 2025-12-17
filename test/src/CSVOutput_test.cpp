#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/Problem.h"
#include "godzilla/CSVOutput.h"
#include "godzilla/Postprocessor.h"

using namespace godzilla;

namespace {

class CSVOutputTest : public FENonlinearProblemTest {};

class TestCSVOutput : public CSVOutput {
public:
    explicit TestCSVOutput(const Parameters & pars) : CSVOutput(pars) {}
    void
    close()
    {
        close_file();
    }
};

} // namespace

TEST_F(CSVOutputTest, get_file_ext)
{
    auto params = CSVOutput::parameters();
    params.set<App *>("app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<String>("file", "asdf");
    CSVOutput out(params);
    out.create();

    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}

TEST_F(CSVOutputTest, create)
{
    auto params = CSVOutput::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("file", "asdf");
    auto out = prob->add_output<TestCSVOutput>(params);

    this->prob->create();

    auto pps_names = out->get_pps_names();
    EXPECT_EQ(pps_names.size(), 0);
}

TEST_F(CSVOutputTest, output)
{
    class TestPostprocessor : public Postprocessor {
    public:
        explicit TestPostprocessor(const Parameters & pars) : Postprocessor(pars) {}

        void compute() override {};

        std::vector<Real>
        get_value() override
        {
            return { 1. };
        }
    };

    auto pp_params = Postprocessor::parameters();
    pp_params.set<String>("name", "pp");
    pp_params.set<App *>("app", this->app);
    this->prob->add_postprocessor<TestPostprocessor>(pp_params);

    auto params = CSVOutput::parameters();
    params.set<App *>("app", this->app);
    params.set<String>("file", "out");
    auto out = this->prob->add_output<TestCSVOutput>(params);

    this->prob->create();

    auto pps_names = out->get_pps_names();
    EXPECT_EQ(pps_names.size(), 1);

    out->output_step();
    out->close();

    std::ifstream f;
    f.open("out.csv");
    if (f.is_open()) {
        String line;

        // FIXME
        // std::getline(f, line);
        // EXPECT_EQ(line, "time,pp");

        // std::getline(f, line);
        // EXPECT_EQ(line, "0,1");

        f.close();
    }
}

TEST_F(CSVOutputTest, set_file_name)
{
    auto params = CSVOutput::parameters();
    params.set<App *>("app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<String>("file", "asdf");
    CSVOutput out(params);

    this->prob->create();
    out.create();

    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}
