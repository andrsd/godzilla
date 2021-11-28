#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "Grid.h"
#include "Problem.h"
#include "VTKOutput.h"
#include "GodzillaApp_test.h"

class VTKOutputTest : public GodzillaAppTest {
protected:
    Grid *
    gGrid()
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 2;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Problem *
    gProblem(Grid * grid)
    {
        const std::string class_name = "GTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    VTKOutput *
    gOutput(Problem * problem, const std::string & file_name)
    {
        const std::string class_name = "VTKOutput";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Problem *>("_problem") = problem;
        params.set<std::string>("file") = file_name;
        return this->app->buildObject<VTKOutput>(class_name, "out", params);
    }
};
