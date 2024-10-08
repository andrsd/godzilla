#pragma once

#include "godzilla/Problem.h"
#include "godzilla/CSVOutput.h"
#include "FENonlinearProblem_test.h"

class CSVOutputTest : public FENonlinearProblemTest {
protected:
    CSVOutput *
    build_output(const std::string & file_name)
    {
        Parameters * params = this->app->get_parameters("CSVOutput");
        params->set<Problem *>("_problem") = this->prob;
        params->set<std::string>("file") = file_name;
        return this->app->build_object<CSVOutput>("out", params);
    }
};
