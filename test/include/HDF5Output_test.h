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
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Problem *>("_problem") = problem;
        params->set<std::string>("file") = file_name;
        return this->app->build_object<HDF5Output>(class_name, "out", params);
    }
};
