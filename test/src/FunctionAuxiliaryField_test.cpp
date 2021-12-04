#include "GodzillaApp_test.h"
#include "FENonlinearProblem_test.h"
#include "InputParameters.h"
#include "LineMesh.h"
#include "FunctionAuxiliaryField.h"

using namespace godzilla;

TEST(FunctionAuxiliaryFieldTest, create)
{
    TestApp app;

    InputParameters grid_params = LineMesh::validParams();
    grid_params.set<const App *>("_app") = &app;
    grid_params.set<PetscInt>("nx") = 2;
    LineMesh grid(grid_params);

    InputParameters prob_params = GTestFENonlinearProblem::validParams();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<Grid *>("_grid") = &grid;
    GTestFENonlinearProblem prob(prob_params);
    prob.addAuxFE(0, "aux1", 1, 1);

    InputParameters aux_params = FunctionAuxiliaryField::validParams();
    aux_params.set<const App *>("_app") = &app;
    aux_params.set<std::string>("_name") = "aux1";
    aux_params.set<FEProblemInterface *>("_fepi") = &prob;
    aux_params.set<std::vector<std::string>>("value") = { "1234" };
    FunctionAuxiliaryField aux(aux_params);
    prob.addAuxiliaryField(&aux);

    grid.create();
    prob.create();

    EXPECT_EQ(aux.getFieldId(), 0);

    EXPECT_EQ(aux.getNumComponents(), 1);

    PetscInt dim = 1;
    PetscReal time = 0;
    PetscReal x[1] = { 1. };
    PetscInt nc = 1;
    PetscReal u[1] = { 0 };
    aux.evaluate(dim, time, x, nc, u);
    EXPECT_EQ(u[0], 1234.);
}
