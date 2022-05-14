#pragma once

#include "Problem.h"
#include "VTKOutput.h"
#include "LinearProblem_test.h"

class VTKOutputTest : public LinearProblemTest {
protected:
    VTKOutput *
    gOutput(Problem * problem, const std::string & file_name)
    {
        const std::string class_name = "VTKOutput";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<Problem *>("_problem") = problem;
        params->set<std::string>("file") = file_name;
        return this->app->build_object<VTKOutput>(class_name, "out", params);
    }
};
