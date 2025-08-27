#include "gmock/gmock.h"
#include "godzilla/Factory.h"
#include "TestApp.h"
#include "godzilla/LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "godzilla/BasicTSAdapt.h"

using namespace godzilla;

TEST(BasicTSAdapt, api)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    // clang-format off
    mesh_pars
        .set<App *>("_app", &app)
        .set<Int>("nx", 2);
    // clang-format on
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestImplicitFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app)
        .set<MeshObject *>("_mesh_obj", &mesh)
        .set<Real>("start_time", 0.)
        .set<Real>("end_time", 1)
        .set<Real>("dt", 0.1);
    GTestImplicitFENonlinearProblem prob(prob_pars);

    app.set_problem(&prob);

    auto params = BasicTSAdapt::parameters();
    // clang-format off
    params
        .set<App *>("_app", &app)
        .set<Problem *>("_problem", &prob);
    // clang-format on
    BasicTSAdapt adaptor(params);
    prob.set_time_stepping_adaptor(&adaptor);

    mesh.create();
    prob.create();

    TSAdapt ts_adapt = adaptor.get_ts_adapt();
    TSAdaptType type;
    TSAdaptGetType(ts_adapt, &type);
    EXPECT_STREQ(type, TSADAPTBASIC);
}
