#include "gtest/gtest.h"
#include "godzilla/Factory.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/FunctionInitialCondition.h"

using namespace godzilla;

TEST(FunctionICTest, api)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem prob(prob_pars);

    Parameters params = FunctionInitialCondition::parameters();
    params.set<App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &prob;
    params.set<std::vector<std::string>>("value") = { "t * (x + y + z)" };
    FunctionInitialCondition obj(params);

    mesh.create();
    prob.create();
    obj.create();

    EXPECT_EQ(obj.get_field_id(), 0);
    EXPECT_EQ(obj.get_num_components(), 1);

    Int dim = 3;
    Real time = 2.;
    Real x[] = { 1, 2, 3 };
    Int Nc = 1;
    Scalar u[] = { 0 };
    obj.evaluate(dim, time, x, Nc, u);

    EXPECT_EQ(u[0], 12);
}
