#pragma once

#include "Problem.h"
#include "CSVOutput.h"
#include "FENonlinearProblem_test.h"

class CSVOutputTest : public FENonlinearProblemTest {
protected:
    CSVOutput *
    build_output(const std::string & file_name)
    {
        const std::string class_name = "CSVOutput";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Problem *>("_problem") = this->prob;
        params->set<std::string>("file") = file_name;
        return this->app->build_object<CSVOutput>(class_name, "out", params);
    }
};