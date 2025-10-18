#include "gtest/gtest.h"
#include "godzilla/Factory.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/BoxMesh.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/FunctionInitialCondition.h"

using namespace godzilla;

TEST(FunctionICTest, api)
{
    TestApp app;

    auto mesh_pars = BoxMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 2);
    mesh_pars.set<Int>("nz", 2);
    auto mesh = MeshFactory::create<BoxMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);
    app.set_problem(&prob);

    auto params = FunctionInitialCondition::parameters();
    params.set<App *>("_app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<std::string>>("value", { "t * (x + y + z)" });
    FunctionInitialCondition obj(params);

    prob.create();
    obj.create();

    EXPECT_EQ(obj.get_field_id(), FieldID(0));
    EXPECT_EQ(obj.get_num_components(), 1);

    Real time = 2.;
    Real x[] = { 1, 2, 3 };
    Scalar u[] = { 0 };
    obj.evaluate(time, x, u);

    EXPECT_EQ(u[0], 12);
}
