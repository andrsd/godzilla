#include "gmock/gmock.h"
#include "Factory.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "BasicTSAdapt.h"

using namespace godzilla;

TEST(BasicTSAdapt, api)
{
    TestApp app;

    LineMesh * mesh;
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    GTestImplicitFENonlinearProblem * prob;
    {
        const std::string class_name = "GTestImplicitFENonlinearProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 1;
        params->set<Real>("dt") = 0.1;
        prob = app.build_object<GTestImplicitFENonlinearProblem>(class_name, "prob", params);
    }
    app.problem = prob;

    Parameters params = BasicTSAdapt::parameters();
    params.set<const App *>("_app") = &app;
    params.set<const Problem *>("_problem") = prob;
    params.set<const TransientProblemInterface *>("_tpi") = prob;
    BasicTSAdapt adaptor(params);
    prob->set_time_stepping_adaptor(&adaptor);

    mesh->create();
    prob->create();

    TSAdapt ts_adapt = adaptor.get_ts_adapt();
    TSAdaptType type;
    TSAdaptGetType(ts_adapt, &type);
    EXPECT_STREQ(type, TSADAPTBASIC);
}