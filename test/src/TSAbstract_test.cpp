#include "gmock/gmock.h"
#include "godzilla/TSAbstract.h"
#include "godzilla/LineMesh.h"
#include "godzilla/FENonlinearProblem.h"
#include "godzilla/TransientProblemInterface.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestTSScheme : public TSAbstract {
public:
    explicit TestTSScheme(TS ts) : TSAbstract(ts) {}
    MOCK_METHOD(void, destroy, ());
    MOCK_METHOD(void, reset, ());
    MOCK_METHOD(void, step, ());
    MOCK_METHOD(void, evaluate_step, (Int order, Vector & X, bool * done));
    MOCK_METHOD(void, rollback, ());
    MOCK_METHOD(void, view, (PetscViewer viewer));

    const Vector &
    get_vec_sol()
    {
        return this->vec_sol;
    }

public:
    static const std::string name;
};

const std::string TestTSScheme::name = "test-ts";

bool
register_scheme()
{
    godzilla::register_ts<TestTSScheme>();
    return true;
}

class GTestProblem : public Problem, public TransientProblemInterface {
public:
    explicit GTestProblem(const Parameters & params) :
        Problem(params),
        TransientProblemInterface(this, params)
    {
    }

    void
    create() override
    {
        Problem::create();
        TransientProblemInterface::init();
        set_up_time_scheme();
    }

    void
    run() override
    {
        auto comm = get_comm();
        auto ts = get_ts();
        auto scheme = static_cast<TestTSScheme *>(ts->data);

        Vector x = Vector::create_seq(comm, 3);
        TSSetSolution(ts, x);
        TSSetMaxSteps(ts, 1);

        TSReset(ts);
        TSStep(ts);
        PetscBool done;
        TSEvaluateStep(ts, 1, x, &done);
        TSRollBack(ts);
        TSView(ts, PETSC_VIEWER_STDOUT_SELF);

        scheme->pre_stage(1.0);

        auto stage_vecs = scheme->get_stage_vectors();
        scheme->post_stage(1.0, 0, stage_vecs);

        scheme->set_cfl_time_local(12.34);

        auto U = Vector::create_seq(comm, 3);
        auto y = Vector::create_seq(comm, 3);
        scheme->compute_rhs_function(0., U, y);
    }

    void
    set_up_time_scheme() override
    {
        set_scheme(TestTSScheme::name);
    }

    void
    pre_stage(Real stage_time) override
    {
        pre_stage_called = true;
    }

    void
    post_stage(Real stage_time, Int stage_index, const std::vector<Vector> & Y) override
    {
        post_stage_called = true;
    }

    ErrorCode
    compute_rhs(Real time, const Vector & vec_x, Vector & vec_F) override
    {
        compute_rhs_called = true;
        return 0;
    }

public:
    static Parameters parameters();

    static bool compute_rhs_called;
    static bool pre_stage_called;
    static bool post_stage_called;
};

bool GTestProblem::compute_rhs_called = false;
bool GTestProblem::pre_stage_called = false;
bool GTestProblem::post_stage_called = false;

Parameters
GTestProblem::parameters()
{
    Parameters params = Problem::parameters();
    params += TransientProblemInterface::parameters();
    params.add_param<std::string>("scheme", "test-ts", "");
    return params;
}

} // namespace

TEST(TSAbstract, test)
{
    static bool dummy = register_scheme();

    testing::internal::CaptureStdout();

    TestApp app;

    auto pars_mesh = LineMesh::parameters();
    pars_mesh.set<godzilla::App *>("_app") = &app;
    pars_mesh.set<Int>("nx") = 2;
    LineMesh mesh(pars_mesh);

    auto pars_prob = GTestProblem::parameters();
    pars_prob.set<godzilla::App *>("_app") = &app;
    pars_prob.set<MeshObject *>("_mesh_obj") = &mesh;
    pars_prob.set<Int>("num_steps") = 1;
    GTestProblem prob(pars_prob);

    mesh.create();
    prob.create();

    auto ts = prob.get_ts();
    auto scheme = static_cast<TestTSScheme *>(ts->data);
    EXPECT_CALL(*scheme, reset).Times(3);
    EXPECT_CALL(*scheme, step);
    EXPECT_CALL(*scheme, evaluate_step);
    EXPECT_CALL(*scheme, rollback);
    EXPECT_CALL(*scheme, view);
    EXPECT_CALL(*scheme, destroy);

    prob.run();

    const auto c_scheme = static_cast<const TestTSScheme *>(ts->data);
    auto stage_vecs = scheme->get_stage_vectors();
    auto c_stage_vecs = c_scheme->get_stage_vectors();
    EXPECT_EQ(stage_vecs.size(), 0);
    EXPECT_EQ(c_stage_vecs.size(), 0);

    PetscReal cfl;
    TSGetCFLTime(ts, &cfl);
    EXPECT_DOUBLE_EQ(cfl, 12.34);

    EXPECT_TRUE(GTestProblem::pre_stage_called);
    EXPECT_TRUE(GTestProblem::post_stage_called);
    EXPECT_TRUE(GTestProblem::compute_rhs_called);

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("type: test-ts"));
}
