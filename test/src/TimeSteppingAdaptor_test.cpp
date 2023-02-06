#include "gmock/gmock.h"
#include "Factory.h"
#include "TestApp.h"
#include "LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "BoundaryCondition.h"
#include "TimeSteppingAdaptor.h"
#include "petsc/private/tsimpl.h"

using namespace godzilla;

static const char * TS_ADAPT_TEST = "test";

static PetscErrorCode __ts_adapt_choose(TSAdapt adapt,
                                        TS ts,
                                        Real h,
                                        Int * next_sc,
                                        Real * next_h,
                                        PetscBool * accept,
                                        Real * wlte,
                                        Real * wltea,
                                        Real * wlter);

PETSC_EXTERN PetscErrorCode
TSAdaptCreate_test(TSAdapt adapt)
{
    adapt->ops->choose = __ts_adapt_choose;
    return 0;
}

///

namespace {

class TestTSAdaptor : public TimeSteppingAdaptor {
public:
    explicit TestTSAdaptor(const Parameters & params);

    virtual void choose(Real h,
                        Int * next_sc,
                        Real * next_h,
                        PetscBool * accept,
                        Real * wlte,
                        Real * wltea,
                        Real * wlter);

    std::vector<Real> dts;

    static Parameters parameters();

protected:
    virtual void
    set_type() override
    {
        PETSC_CHECK(TSAdaptSetType(this->ts_adapt, TS_ADAPT_TEST));
    }
};

} // namespace

REGISTER_OBJECT(TestTSAdaptor);

Parameters
TestTSAdaptor::parameters()
{
    Parameters pars = TimeSteppingAdaptor::parameters();
    return pars;
}

TestTSAdaptor::TestTSAdaptor(const Parameters & params) : TimeSteppingAdaptor(params)
{
    this->dts = { 0.2, 0.3, 0.4, 1 };
}

void
TestTSAdaptor::choose(Real h,
                      Int * next_sc,
                      Real * next_h,
                      PetscBool * accept,
                      Real * wlte,
                      Real * wltea,
                      Real * wlter)
{
    Int idx = this->problem->get_step_num();
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

static PetscErrorCode
__ts_adapt_choose(TSAdapt adapt,
                  TS ts,
                  Real h,
                  Int * next_sc,
                  Real * next_h,
                  PetscBool * accept,
                  Real * wlte,
                  Real * wltea,
                  Real * wlter)
{
    void * ctx;
    TSGetApplicationContext(ts, &ctx);
    TransientProblemInterface * tpi = static_cast<TransientProblemInterface *>(ctx);
    TestTSAdaptor * adaptor = dynamic_cast<TestTSAdaptor *>(tpi->get_time_stepping_adaptor());
    assert(adaptor != nullptr);
    adaptor->choose(h, next_sc, next_h, accept, wlte, wltea, wlter);
    return 0;
}

///

class TestTSProblem : public GTestImplicitFENonlinearProblem {
public:
    explicit TestTSProblem(const Parameters & params);

    std::vector<Real> dts;

protected:
    virtual PetscErrorCode ts_monitor_callback(Int stepi, Real time, Vec x);
};

REGISTER_OBJECT(TestTSProblem);

TestTSProblem::TestTSProblem(const Parameters & params) : GTestImplicitFENonlinearProblem(params)
{
    this->dts.resize(5);
}

PetscErrorCode
TestTSProblem::ts_monitor_callback(Int stepi, Real time, Vec x)
{
    Real dt;
    PETSC_CHECK(TSGetTimeStep(this->ts, &dt));
    this->dts[stepi] = dt;
    return 0;
}

///

TEST(TimeSteppingAdaptor, api)
{
    class MockTSAdaptor : public TimeSteppingAdaptor {
    public:
        explicit MockTSAdaptor(const Parameters & params) : TimeSteppingAdaptor(params) {}

        MOCK_METHOD((void), set_type, ());
    };

    TestApp app;

    LineMesh * mesh;
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    TestTSProblem * prob;
    {
        const std::string class_name = "TestTSProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 1;
        params->set<Real>("dt") = 0.1;
        prob = app.build_object<TestTSProblem>(class_name, "prob", params);
    }
    app.problem = prob;
    mesh->create();
    prob->create();

    Parameters params = MockTSAdaptor::parameters();
    params.set<const App *>("_app") = &app;
    params.set<const Problem *>("_problem") = prob;
    params.set<const TransientProblemInterface *>("_tpi") = prob;
    params.set<Real>("dt_min") = 1e-3;
    params.set<Real>("dt_max") = 1e3;
    MockTSAdaptor adaptor(params);

    EXPECT_DOUBLE_EQ(adaptor.get_dt_min(), 1e-3);
    EXPECT_DOUBLE_EQ(adaptor.get_dt_max(), 1e3);

    EXPECT_CALL(adaptor, set_type);
    adaptor.create();
}

TEST(TimeSteppingAdaptor, choose)
{
    TestApp app;
    PETSC_CHECK(TSAdaptRegister(TS_ADAPT_TEST, TSAdaptCreate_test));

    LineMesh * mesh;
    {
        const std::string class_name = "LineMesh";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<Int>("nx") = 2;
        mesh = app.build_object<LineMesh>(class_name, "mesh", params);
    }

    TestTSProblem * prob;
    {
        const std::string class_name = "TestTSProblem";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Mesh *>("_mesh") = mesh;
        params->set<Real>("start_time") = 0.;
        params->set<Real>("end_time") = 1;
        params->set<Real>("dt") = 0.1;
        prob = app.build_object<TestTSProblem>(class_name, "prob", params);
    }
    app.problem = prob;

    {
        const std::string class_name = "DirichletBC";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<std::string>("boundary") = "marker";
        params->set<std::vector<std::string>>("value") = { "x*x" };
        params->set<const DiscreteProblemInterface *>("_dpi") = prob;
        auto bc = app.build_object<BoundaryCondition>(class_name, "bc", params);
        prob->add_boundary_condition(bc);
    }

    {
        const std::string class_name = "TestTSAdaptor";
        Parameters * params = Factory::get_parameters(class_name);
        params->set<const Problem *>("_problem") = prob;
        params->set<const TransientProblemInterface *>("_tpi") = prob;
        auto * ts_adaptor = app.build_object<TestTSAdaptor>(class_name, "ts_adapt", params);
        prob->set_time_stepping_adaptor(ts_adaptor);
    }

    mesh->create();
    prob->create();

    prob->run();

    EXPECT_THAT(prob->dts, testing::ElementsAre(0.1, 0.2, 0.3, 0.4, 0.));
}
