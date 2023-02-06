#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "LineMesh.h"
#include "DirichletBC.h"
#include "GTestFENonlinearProblem.h"
#include "L2Diff.h"

TEST(L2DiffTest, compute)
{
    TestApp app;

    Parameters mesh_params = LineMesh::parameters();
    mesh_params.set<const App *>("_app") = &app;
    mesh_params.set<Int>("nx") = 2;
    LineMesh mesh(mesh_params);

    Parameters prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<const App *>("_app") = &app;
    prob_params.set<const Mesh *>("_mesh") = &mesh;
    GTestFENonlinearProblem prob(prob_params);
    app.problem = &prob;

    Parameters bc_params = DirichletBC::parameters();
    bc_params.set<const App *>("_app") = &app;
    bc_params.set<const DiscreteProblemInterface *>("_dpi") = &prob;
    bc_params.set<std::vector<std::string>>("value") = { "x*x" };
    bc_params.set<std::string>("boundary") = "marker";
    DirichletBC bc(bc_params);

    Parameters ps_params = L2Diff::parameters();
    ps_params.set<const App *>("_app") = &app;
    ps_params.set<const Problem *>("_problem") = &prob;
    ps_params.set<std::vector<std::string>>("value") = { "x*x" };
    L2Diff ps(ps_params);

    prob.add_boundary_condition(&bc);
    prob.add_postprocessor(&ps);

    mesh.create();
    prob.create();

    prob.solve();
    prob.compute_postprocessors();

    PetscReal l2_err = ps.get_value();
    EXPECT_NEAR(l2_err, 0.0416667, 1e-7);
}
