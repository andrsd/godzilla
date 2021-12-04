#pragma once

#include "Problem.h"
#include "ExodusIIOutput.h"
#include "LinearProblem_test.h"

class ExodusIIOutputTest : public LinearProblemTest {
protected:
    ExodusIIOutput *
    gOutput(Problem * problem, const std::string & file_name)
    {
        const std::string class_name = "ExodusIIOutput";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Problem *>("_problem") = problem;
        params.set<std::string>("file") = file_name;
        return this->app->buildObject<ExodusIIOutput>(class_name, "out", params);
    }

    Problem *
    gFEProblem1d(Grid * grid)
    {
        const std::string class_name = "GTestFENonlinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }
};
