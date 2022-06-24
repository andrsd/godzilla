#include "gmock/gmock.h"
#include "Factory.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "BoundaryCondition.h"
#include "TimeSteppingAdaptor.h"
#include "petsc/private/tsimpl.h"

using namespace godzilla;

class TestTSAdaptor : public TimeSteppingAdaptor {
public:
    explicit TestTSAdaptor(const InputParameters & params);

    virtual void choose(PetscReal h,
                        PetscInt * next_sc,
                        PetscReal * next_h,
                        PetscBool * accept,
                        PetscReal * wlte,
                        PetscReal * wltea,
                        PetscReal * wlter);

    std::vector<PetscReal> dts;

    static InputParameters valid_params();
};

registerObject(TestTSAdaptor);

InputParameters
TestTSAdaptor::valid_params()
{
    InputParameters pars = TimeSteppingAdaptor::valid_params();
    return pars;
}

TestTSAdaptor::TestTSAdaptor(const InputParameters & params) : TimeSteppingAdaptor(params)
{
    this->dts = { 0.2, 0.3, 0.4, 1 };
}

void
TestTSAdaptor::choose(PetscReal h,
                      PetscInt * next_sc,
                      PetscReal * next_h,
                      PetscBool * accept,
                      PetscReal * wlte,
                      PetscReal * wltea,
                      PetscReal * wlter)
{
    PetscInt idx = this->problem->get_step_num();
    *next_sc = 0;
    if (idx < 3) {
        *next_h = this->dts[idx];
        *accept = PETSC_TRUE;
    }
    else {
        *next_h = 0;
        *accept = PETSC_TRUE;
    }
    *wlte = -1;
    *wltea = -1;
    *wlter = -1;
}

///

class TestTSProblem : public GTestImplicitFENonlinearProblem {
public:
    explicit TestTSProblem(const InputParameters & params);

    std::vector<PetscReal> dts;

protected:
    virtual PetscErrorCode ts_monitor_callback(PetscInt stepi, PetscReal time, Vec x);
};

registerObject(TestTSProblem);

TestTSProblem::TestTSProblem(const InputParameters & params) :
    GTestImplicitFENonlinearProblem(params)
{
    this->dts.resize(5);
}

PetscErrorCode
TestTSProblem::ts_monitor_callback(PetscInt stepi, PetscReal time, Vec x)
{
    PetscReal dt;
    PETSC_CHECK(TSGetTimeStep(this->ts, &dt));
    this->dts[stepi] = dt;
    return 0;
}

///

TEST(TimeSteppingAdaptor, choose)
{
    TestApp app;
    TransientProblemInterface::register_types();

    LineMesh * mesh;
    {
        const std::string class_name = "LineMesh";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    TestTSProblem * prob;
    {
        const std::string class_name = "TestTSProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 1;
        params->set<PetscReal>("dt") = 0.1;
        prob = app.build_object<TestTSProblem>(class_name, "prob", params);
    }
    app.problem = prob;

    {
        const std::string class_name = "DirichletBC";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        params->set<const FEProblemInterface *>("_fepi") = prob;
        auto bc = app.build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    TestTSAdaptor * ts_adaptor;
    {
        const std::string class_name = "TestTSAdaptor";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Problem *>("_problem") = prob;
        params->set<const TransientProblemInterface *>("_tpi") = prob;
        ts_adaptor = app.build_object<TestTSAdaptor>(class_name, "ts_adapt", params);
    }

    mesh->create();
    prob->create();

    prob->set_time_stepping_adaptor(ts_adaptor);

    prob->run();

    EXPECT_THAT(prob->dts, testing::ElementsAre(0.1, 0.2, 0.3, 0.4, 0.));
}

TEST(TimeSteppingAdaptor, set_time_stepping_scheme)
{
    TestApp app;

    LineMesh * mesh;
    {
        const std::string class_name = "LineMesh";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<PetscInt>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    TestTSProblem * prob;
    {
        const std::string class_name = "TestTSProblem";
        InputParameters * params = Factory::get_valid_params(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<PetscReal>("start_time") = 0.;
        params->set<PetscReal>("end_time") = 1;
        params->set<PetscReal>("dt") = 0.1;
        prob = app.build_object<TestTSProblem>(class_name, "prob", params);
    }
    app.problem = prob;

    mesh->create();
    prob->create();

    prob->set_time_stepping_scheme(TSADAPTBASIC);

    TSAdapt ts_adapt = prob->get_ts_adapt();

    TSAdaptType type;
    TSAdaptGetType(ts_adapt, &type);

    EXPECT_STREQ(type, TSADAPTBASIC);
}
