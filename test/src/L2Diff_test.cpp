#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "FENonlinearProblem_test.h"
#include "L2Diff.h"

TEST(L2DiffTest, compute)
{
    TestApp app;

    InputParameters mesh_params = LineMesh::validParams();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<PetscInt>("nx") = 2;
    LineMesh mesh(mesh_params);

    InputParameters prob_params = GTestFENonlinearProblem::validParams();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);

    InputParameters bc_params = DirichletBC::validParams();
    bc_params.set<const App *>("_app") = &app;
    bc_params.set<std::vector<std::string>>("value") = { "x*x" };
    bc_params.set<std::string>("boundary") = "marker";
    DirichletBC bc(bc_params);

    InputParameters ps_params = L2Diff::validParams();
    ps_params.set<const App *>("_app") = &app;
    ps_params.set<Problem *>("_problem") = &prob;
    ps_params.set<std::vector<std::string>>("value") = { "x*x" };
    L2Diff ps(ps_params);

    prob.addBoundaryCondition(&bc);
    prob.addPostprocessor(&ps);

    mesh.create();
    prob.create();

    prob.solve();
    prob.computePostprocessors();

    PetscReal l2_err = ps.getValue();
    EXPECT_NEAR(l2_err, 0.0416667, 1e-7);
}
