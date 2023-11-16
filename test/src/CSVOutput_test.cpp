#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "CSVOutput_test.h"
#include "Postprocessor.h"

namespace {

class TestCSVOutput : public CSVOutput {
public:
    explicit TestCSVOutput(const Parameters & pars) : CSVOutput(pars) {}
    void
    close()
    {
        close_file();
    }

    std::vector<std::string>
    get_pps_names()
    {
        return pps_names;
    }

    const std::FILE *
    get_f()
    {
        return f;
    }
};

} // namespace

TEST_F(CSVOutputTest, get_file_ext)
{
    Parameters params = CSVOutput::parameters();
    params.set<App *>("_app") = this->app;
    params.set<Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    CSVOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "csv");
}

TEST_F(CSVOutputTest, create)
{
    Parameters params = CSVOutput::parameters();
    params.set<App *>("_app") = this->app;
    params.set<Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    TestCSVOutput out(params);
    prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 0);

    EXPECT_TRUE(out.get_f() == nullptr);
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

    Parameters pp_params = Postprocessor::parameters();
    pp_params.set<std::string>("_name") = "pp";
    pp_params.set<App *>("_app") = this->app;
    pp_params.set<Problem *>("_problem") = this->prob;
    TestPostprocessor pp(pp_params);
    this->prob->add_postprocessor(&pp);

    Parameters params = CSVOutput::parameters();
    params.set<App *>("_app") = this->app;
    params.set<Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    TestCSVOutput out(params);
    this->prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 1);

    EXPECT_TRUE(out.get_f() != nullptr);

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
    Parameters params = CSVOutput::parameters();
    params.set<App *>("_app") = this->app;
    params.set<Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    CSVOutput out(params);

    this->mesh->create();
    this->prob->create();

    out.set_file_name();
    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}
