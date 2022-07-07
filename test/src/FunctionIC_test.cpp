#include "gtest/gtest.h"
#include "Factory.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "FunctionIC.h"

using namespace godzilla;

TEST(FunctionICTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<const App *>("_app") = &app;
    mesh_pars.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<const App *>("_app") = &app;
    prob_pars.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = FunctionIC::parameters();
    params.set<const App *>("_app") = &app;
    params.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    FunctionIC obj(params);

    mesh.create();
    prob.create();
    obj.create();

    EXPECT_EQ(obj.get_field_id(), 0);
    EXPECT_EQ(obj.get_num_components(), 1);

    PetscInt dim = 3;
    PetscReal time = 2.;
    PetscReal x[] = { 1, 2, 3 };
    PetscInt Nc = 1;
    PetscScalar u[] = { 0 };
    obj.evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 12);
}
