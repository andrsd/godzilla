#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "Parameters.h"
#include "LineMesh.h"
#include "ConstantAuxiliaryField.h"

using namespace godzilla;

TEST(ConstantAuxiliaryFieldTest, create)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    prob.add_aux_fe(0, "aux1", 1, 1);

    Parameters aux_params = ConstantAuxiliaryField::parameters();
    aux_params.set<const App *>("_app") = &app;
    aux_params.set<std::string>("_name") = "aux1";
    aux_params.set<FEProblemInterface *>("_fepi") = &prob;
    aux_params.set<std::vector<PetscReal>>("value") = { 1234 };
    ConstantAuxiliaryField aux(aux_params);
    prob.add_auxiliary_field(&aux);

    mesh.create();
    prob.create();

    EXPECT_EQ(aux.get_field_id(), 0);

    EXPECT_EQ(aux.get_num_components(), 1);

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[1] = { 1. };
    PetscInt nc = 1;
    PetscReal u[1] = { 0 };
    aux.evaluate(dim, time, x, nc, u);
    EXPECT_EQ(u[0], 1234.);
}

TEST(ConstantAuxiliaryFieldTest, evaluate)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    prob.add_aux_fe(0, "aux1", 1, 1);

    Parameters aux_params = ConstantAuxiliaryField::parameters();
    aux_params.set<const App *>("_app") = &app;
    aux_params.set<std::string>("_name") = "aux1";
    aux_params.set<FEProblemInterface *>("_fepi") = &prob;
    aux_params.set<std::vector<PetscReal>>("value") = { 1234 };
    ConstantAuxiliaryField aux(aux_params);
    prob.add_auxiliary_field(&aux);

    mesh.create();
    prob.create();

    PetscFunc * fn = aux.get_func();
    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[1] = { 1. };
    PetscInt nc = 1;
    PetscReal u[1] = { 0 };
    (*fn)(dim, time, x, nc, u, &aux);
    EXPECT_EQ(u[0], 1234.);
}
