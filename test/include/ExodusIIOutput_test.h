#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "Grid.h"
#include "Problem.h"
#include "ExodusIIOutput.h"
#include "GodzillaApp_test.h"

class ExodusIIOutputTest : public GodzillaAppTest {
protected:
    Grid *
    gGrid1d()
    {
        const std::string class_name = "LineMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Grid *
    gGrid2d()
    {
        const std::string class_name = "RectangleMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Grid *
    gGrid3d()
    {
        const std::string class_name = "BoxMesh";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<PetscInt>("nx") = 1;
        params.set<PetscInt>("ny") = 1;
        params.set<PetscInt>("nz") = 1;
        return this->app->buildObject<Grid>(class_name, "grid", params);
    }

    Problem *
    gProblem1d(Grid * grid)
    {
        const std::string class_name = "G1DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem2d(Grid * grid)
    {
        const std::string class_name = "G2DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    Problem *
    gProblem3d(Grid * grid)
    {
        const std::string class_name = "G3DTestPetscLinearProblem";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Grid *>("_grid") = grid;
        return this->app->buildObject<Problem>(class_name, "problem", params);
    }

    ExodusIIOutput *
    gOutput(Problem * problem, const std::string & file_name)
    {
        const std::string class_name = "ExodusIIOutput";
        InputParameters & params = Factory::getValidParams(class_name);
        params.set<Problem *>("_problem") = problem;
        params.set<std::string>("file") = file_name;
        return this->app->buildObject<ExodusIIOutput>(class_name, "out", params);
    }
};
