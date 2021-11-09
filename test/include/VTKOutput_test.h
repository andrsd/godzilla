#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "GGrid.h"
#include "GExecutioner.h"
#include "GProblem.h"
#include "VTKOutput.h"
#include "GodzillaApp_test.h"


class VTKOutputTest : public GodzillaAppTest {
protected:
    GGrid *
    gGrid()
    {
        const std::string class_name = "G1DLineMesh";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<PetscInt>("nx") = 2;
        return Factory::create<GGrid>(class_name, "grid", params);
    }

    GProblem *
    gProblem(GGrid *grid)
    {
        const std::string class_name = "GTestPetscLinearProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<GGrid *>("_ggrid") = grid;
        return Factory::create<GProblem>(class_name, "problem", params);
    }

    GExecutioner *
    gExecutioner(GProblem * problem)
    {
        const std::string class_name = "GExecutioner";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<GProblem *>("_gproblem") = problem;
        return Factory::create<GExecutioner>(class_name, "obj", params);
    }

    VTKOutput *
    gOutput(GProblem * problem, const std::string & file_name)
    {
        const std::string class_name = "VTKOutput";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<GProblem *>("_gproblem") = problem;
        params.set<std::string>("file") = file_name;
        return Factory::create<VTKOutput>(class_name, "out", params);
    }
};
