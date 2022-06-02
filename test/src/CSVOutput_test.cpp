#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "CSVOutput_test.h"
#include "Postprocessor.h"
#include "petsc.h"

class TestCSVOutput : public CSVOutput {
public:
    TestCSVOutput(const InputParameters & pars) : CSVOutput(pars) {}
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

    const std::ofstream &
    get_f()
    {
        return f;
    }
};

TEST_F(CSVOutputTest, get_file_ext)
{
    InputParameters params = CSVOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    CSVOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "csv");
}

TEST_F(CSVOutputTest, create)
{
    InputParameters params = CSVOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    TestCSVOutput out(params);
    prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 0);

    EXPECT_FALSE(out.get_f().is_open());
}

TEST_F(CSVOutputTest, output)
{
    class TestPostprocessor : public Postprocessor {
    public:
        TestPostprocessor(const InputParameters & pars) : Postprocessor(pars) {}
        virtual void compute() {};
        virtual PetscReal
        get_value()
        {
            return 1.;
        }
    };

    InputParameters pp_params = Postprocessor::valid_params();
    pp_params.set<std::string>("_name") = "pp";
    pp_params.set<const App *>("_app") = this->app;
    pp_params.set<const Problem *>("_problem") = this->prob;
    TestPostprocessor pp(pp_params);
    this->prob->add_postprocessor(&pp);

    InputParameters params = CSVOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    TestCSVOutput out(params);
    this->prob->add_output(&out);

    this->mesh->create();
    this->prob->create();

    auto pps_names = out.get_pps_names();
    EXPECT_EQ(pps_names.size(), 1);

    EXPECT_TRUE(out.get_f().is_open());

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
    InputParameters params = CSVOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "asdf";
    CSVOutput out(params);

    this->mesh->create();
    this->prob->create();

    out.set_file_name();
    EXPECT_EQ(out.get_file_name(), "asdf.csv");
}
