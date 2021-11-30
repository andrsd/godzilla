#pragma once

#include "Grid.h"
#include "Problem.h"
#include "GodzillaApp_test.h"

class OutputTest : public GodzillaAppTest {
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
};
