#pragma once

#include "NonlinearProblem.h"
#include "GodzillaApp_test.h"

using namespace godzilla;

class G1DTestNonlinearProblem;

class NonlinearProblemTest : public GodzillaAppTest {
protected:
    Mesh *
    gMesh1d()
    {
        const std::string class_name = "LineMesh";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 1;
        return this->app->build_object<Mesh>(class_name, "mesh", params);
    }

    G1DTestNonlinearProblem *
    gProblem1d(Mesh * mesh)
    {
        const std::string class_name = "G1DTestNonlinearProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        return this->app->build_object<G1DTestNonlinearProblem>(class_name, "problem", params);
    }
};

//

class G1DTestNonlinearProblem : public NonlinearProblem {
public:
    G1DTestNonlinearProblem(const InputParameters & params);
    virtual ~G1DTestNonlinearProblem();
    virtual void create() override;
    void call_initial_guess();

protected:
    virtual PetscErrorCode compute_residual_callback(Vec x, Vec f) override;
    virtual PetscErrorCode compute_jacobian_callback(Vec x, Mat J, Mat Jp) override;

    PetscSection s;
};
