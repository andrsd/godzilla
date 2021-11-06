#pragma once

#include "Factory.h"
#include "gmock/gmock.h"
#include "GExecutioner.h"
#include "GProblem.h"
#include "GVTKOutput.h"
#include "GodzillaApp_test.h"


class GVTKOutputTest : public GodzillaAppTest {
protected:
    GProblem *
    gProblem()
    {
        const std::string class_name = "GTestPetscLinearProblem";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
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

    GVTKOutput *
    gOutput(GProblem * problem, const std::string & file_name)
    {
        const std::string class_name = "GVTKOutput";
        InputParameters params = Factory::getValidParams(class_name);
        params.set<const App *>("_app") = this->app;
        params.set<GProblem *>("_gproblem") = problem;
        params.set<std::string>("file") = file_name;
        return Factory::create<GVTKOutput>(class_name, "out", params);
    }
};
