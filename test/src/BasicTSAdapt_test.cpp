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
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestImplicitFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    prob_pars.set<Real>("start_time") = 0.;
    prob_pars.set<Real>("end_time") = 1;
    prob_pars.set<Real>("dt") = 0.1;
    GTestImplicitFENonlinearProblem prob(prob_pars);

    app.set_problem(&prob);

    Parameters params = BasicTSAdapt::parameters();
    params.set<App *>("_app") = &app;
    params.set<Problem *>("_problem") = &prob;
    BasicTSAdapt adaptor(params);
    prob.set_time_stepping_adaptor(&adaptor);

    mesh.create();
    prob.create();

    TSAdapt ts_adapt = adaptor.get_ts_adapt();
    TSAdaptType type;
    TSAdaptGetType(ts_adapt, &type);
    EXPECT_STREQ(type, TSADAPTBASIC);
}
