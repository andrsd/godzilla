#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "ExodusIIOutput.h"
#include "petsc.h"

using namespace godzilla;

class ExodusIIOutputTest : public FENonlinearProblemTest {
};

TEST_F(ExodusIIOutputTest, get_file_ext)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    EXPECT_EQ(out.get_file_ext(), "exo");
}

TEST_F(ExodusIIOutputTest, create)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    this->prob->add_output(&out);
    out.create();
}

TEST_F(ExodusIIOutputTest, check)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    out.check();
}

TEST_F(ExodusIIOutputTest, output)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    ExodusIIOutput out(params);

    this->mesh->create();
    this->prob->create();

    out.check();
    this->app->check_integrity();

    out.output_step();
}

TEST_F(ExodusIIOutputTest, set_file_name)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    out.set_file_name();
    EXPECT_EQ(out.get_file_name(), "out.exo");
}

TEST_F(ExodusIIOutputTest, set_seq_file_name)
{
    InputParameters params = ExodusIIOutput::valid_params();
    params.set<const App *>("_app") = this->app;
    params.set<const Problem *>("_problem") = this->prob;
    params.set<std::string>("file") = "out";
    ExodusIIOutput out(params);

    out.create();

    out.set_sequence_file_name(2);
    EXPECT_EQ(out.get_file_name(), "out.2.exo");
}
