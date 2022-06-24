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
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 2;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    GTestImplicitFENonlinearProblem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 20;
        params->set<PetscReal>("dt") = 5;
        return this->app->build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
    }
};
