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
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<Problem *>("_problem") = problem;
        params->set<std::string>("file") = file_name;
        return this->app->build_object<ExodusIIOutput>(class_name, "out", params);
    }

    Problem *
    gFEProblem(Mesh * mesh)
    {
        const std::string class_name = "GTestFENonlinearProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<Problem>(class_name, "problem", params);
    }
};
