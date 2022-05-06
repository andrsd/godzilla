#pragma once

#include "Problem.h"
#include "HDF5Output.h"
#include "LinearProblem_test.h"

class HDF5OutputTest : public LinearProblemTest {
protected:
    HDF5Output *
    gOutput(Problem * problem, const std::string & file_name)
    {
        const std::string class_name = "HDF5Output";
        InputParameters * params = Factory::getValidParams(class_name);
        params->set<Problem *>("_problem") = problem;
        params->set<std::string>("file") = file_name;
        return this->app->buildObject<HDF5Output>(class_name, "out", params);
    }
};
