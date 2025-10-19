#include "gmock/gmock.h"
#include "godzilla/Factory.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "GTestImplicitFENonlinearProblem.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/TimeSteppingAdaptor.h"
#include "petsc/private/tsimpl.h"

using namespace godzilla;

static const char * TS_ADAPT_TEST = "test";

static ErrorCode __ts_adapt_choose(TSAdapt adapt,
                                   TS ts,
                                   Real h,
                                   Int * next_sc,
                                   Real * next_h,
                                   PetscBool * accept,
                                   Real * wlte,
                                   Real * wltea,
                                   Real * wlter);

PETSC_EXTERN ErrorCode
TSAdaptCreate_test(TSAdapt adapt)
{
    adapt->ops->choose = __ts_adapt_choose;
    return 0;
}

///

namespace {

class TestTSAdaptor : public TimeSteppingAdaptor {
public:
    explicit TestTSAdaptor(const Parameters & pars);

    void create() override;

    void choose(Real h,
                Int * next_sc,
                Real * next_h,
                PetscBool * accept,
                Real * wlte,
                Real * wltea,
                Real * wlter);

    std::vector<Real> dts;

    static Parameters parameters();
};

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

private:
    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }

    void
    set_up_callbacks() override
    {
        set_compute([](Real time, const Real x[], Scalar u[]) { u[0] = x[0] * x[0]; });
    }
};

} // namespace

Parameters
TestTSAdaptor::parameters()
{
    Parameters pars = TimeSteppingAdaptor::parameters();
    return pars;
}

TestTSAdaptor::TestTSAdaptor(const Parameters & pars) : TimeSteppingAdaptor(pars)
{
    this->dts = { 0.2, 0.3, 0.4, 1 };
}

void
TestTSAdaptor::create()
{
    TimeSteppingAdaptor::create();
    set_type(TS_ADAPT_TEST);
    set_always_accept(true);
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
    Int idx = get_problem()->get_step_num();
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

static ErrorCode
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
    explicit TestTSProblem(const Parameters & pars) : GTestImplicitFENonlinearProblem(pars)
    {
        this->dts.resize(5);
    }

    std::vector<Real> dts;

protected:
    void
    set_up_monitors() override
    {
        monitor_cancel();
        monitor_set(this, &TestTSProblem::ts_monitor);
    }

    void
    ts_monitor(Int stepi, Real time, const Vector & x)
    {
        Real dt = get_time_step();
        this->dts[stepi] = dt;
    }
};

///

TEST(TimeSteppingAdaptor, api)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestTSProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    prob_pars.set<Real>("start_time", 0.);
    prob_pars.set<Real>("end_time", 1);
    prob_pars.set<Real>("dt", 0.1);
    TestTSProblem prob(prob_pars);

    app.set_problem(&prob);
    prob.create();

    auto params = TimeSteppingAdaptor::parameters();
    params.set<App *>("_app", &app);
    params.set<Problem *>("_problem", &prob);
    params.set<Real>("dt_min", 1e-3);
    params.set<Real>("dt_max", 1e3);
    TimeSteppingAdaptor adaptor(params);

    EXPECT_DOUBLE_EQ(adaptor.get_dt_min(), 1e-3);
    EXPECT_DOUBLE_EQ(adaptor.get_dt_max(), 1e3);

    adaptor.create();
}

TEST(TimeSteppingAdaptor, choose)
{
    TestApp app;
    PETSC_CHECK(TSAdaptRegister(TS_ADAPT_TEST, TSAdaptCreate_test));

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = TestTSProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    prob_pars.set<Real>("start_time", 0.);
    prob_pars.set<Real>("end_time", 1);
    prob_pars.set<Real>("dt", 0.1);
    TestTSProblem prob(prob_pars);

    app.set_problem(&prob);

    auto bc_pars = DirichletBC::parameters();
    bc_pars.set<App *>("_app", &app);
    bc_pars.set<std::vector<std::string>>("boundary", { "left", "right" });
    bc_pars.set<DiscreteProblemInterface *>("_dpi", &prob);
    DirichletBC bc(bc_pars);
    prob.add_boundary_condition(&bc);

    auto tsa_pars = TestTSAdaptor::parameters();
    tsa_pars.set<App *>("_app", &app);
    tsa_pars.set<Problem *>("_problem", &prob);
    TestTSAdaptor ts_adaptor(tsa_pars);
    prob.set_time_stepping_adaptor(&ts_adaptor);

    prob.create();
    app.check_integrity();
    app.get_logger()->print();

    prob.run();

    EXPECT_THAT(prob.dts, testing::ElementsAre(0.1, 0.2, 0.3, 0.4, 0.));
}
