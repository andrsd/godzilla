#pragma once

#include "ImplicitFENonlinearProblem.h"
#include "Mesh.h"
#include "InitialCondition.h"
#include "GodzillaApp_test.h"
#include "GTestImplicitFENonlinearProblem.h"

class ImplicitFENonlinearProblemTest : public GodzillaAppTest {
public:
    Mesh *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Int>("nx") = 2;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    GTestImplicitFENonlinearProblem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 20;
        params->set<Real>("dt") = 5;
        return this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
    }
};
