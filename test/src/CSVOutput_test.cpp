#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "godzilla/Problem.h"
#include "godzilla/CSVOutput.h"
#include "godzilla/Postprocessor.h"

namespace {

class CSVOutputTest : public FENonlinearProblemTest {
protected:
    CSVOutput *
    build_output(const std::string & file_name)
    {
        auto * params = this->app->get_parameters("CSVOutput");
        params->set<Problem *>("_problem", this->prob);
        params->set<std::string>("file", file_name);
        return this->app->build_object<CSVOutput>("out", params);
    }
};

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
    params.set<App *>("_app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<std::string>("file", "asdf");
    CSVOutput out(params);

    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}

TEST_F(CSVOutputTest, create)
{
    auto params = CSVOutput::parameters();
    params.set<App *>("_app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<std::string>("file", "asdf");
    TestCSVOutput out(params);
    prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 0);
}

TEST_F(CSVOutputTest, output)
{
    class TestPostprocessor : public Postprocessor {
    public:
        explicit TestPostprocessor(const Parameters & pars) : Postprocessor(pars) {}

        void compute() override {};

        Real
        get_value() override
        {
            return 1.;
        }
    };

    auto pp_params = Postprocessor::parameters();
    pp_params.set<std::string>("_name", "pp")
        .set<App *>("_app", this->app)
        .set<Problem *>("_problem", this->prob);
    TestPostprocessor pp(pp_params);
    this->prob->add_postprocessor(&pp);

    auto params = CSVOutput::parameters();
    params.set<App *>("_app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<std::string>("file", "out");
    TestCSVOutput out(params);
    this->prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 1);

    out.output_step();
    out.close();

    std::ifstream f;
    f.open("out.csv");
    if (f.is_open()) {
        std::string line;

        std::getline(f, line);
        EXPECT_EQ(line, "time,pp");

        std::getline(f, line);
        EXPECT_EQ(line, "0,1");

        f.close();
    }
}

TEST_F(CSVOutputTest, set_file_name)
{
    auto params = CSVOutput::parameters();
    params.set<App *>("_app", this->app)
        .set<Problem *>("_problem", this->prob)
        .set<std::string>("file", "asdf");
    CSVOutput out(params);

    this->mesh->create();
    this->prob->create();

    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}
