#include "gmock/gmock.h"
#include "godzilla/TSAbstract.h"
#include "TestApp.h"

using namespace godzilla;
using namespace testing;

namespace {

bool compute_rhs_called = false;
bool pre_stage_called = false;
bool post_stage_called = false;

PetscErrorCode
compute_rhs(TS, Real time, Vec x, Vec F, void * ctx)
{
    compute_rhs_called = true;
    return 0;
}

PetscErrorCode
pre_stage(TS, Real time)
{
    pre_stage_called = true;
    return 0;
}

PetscErrorCode
post_stage(TS, Real time, Int i, Vec * x)
{
    post_stage_called = true;
    return 0;
}

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

} // namespace

TEST(TSAbstract, test)
{
    static bool dummy = register_scheme();

    testing::internal::CaptureStdout();

    TestApp app;
    auto comm = app.get_comm();

    TS ts;
    TSCreate(comm, &ts);

    TSSetType(ts, TestTSScheme::name.c_str());

    auto scheme = static_cast<TestTSScheme *>(ts->data);
    const auto c_scheme = static_cast<const TestTSScheme *>(ts->data);
    EXPECT_CALL(*scheme, reset).Times(3);
    EXPECT_CALL(*scheme, step);
    EXPECT_CALL(*scheme, evaluate_step);
    EXPECT_CALL(*scheme, rollback);
    EXPECT_CALL(*scheme, view);
    EXPECT_CALL(*scheme, destroy);

    Vector x = Vector::create_seq(comm, 3);
    TSSetSolution(ts, x);
    TSSetMaxSteps(ts, 1);
    TSSetRHSFunction(ts, x, compute_rhs, nullptr);
    TSSetPreStage(ts, pre_stage);
    TSSetPostStage(ts, post_stage);

    TSSetUp(ts);
    EXPECT_EQ(scheme->get_vec_sol(), x);

    TSReset(ts);
    TSStep(ts);
    PetscBool done;
    TSEvaluateStep(ts, 1, x, &done);
    TSRollBack(ts);
    TSView(ts, PETSC_VIEWER_STDOUT_SELF);

    auto stage_vecs = scheme->get_stage_vectors();
    auto c_stage_vecs = c_scheme->get_stage_vectors();
    EXPECT_EQ(stage_vecs.size(), 0);
    EXPECT_EQ(c_stage_vecs.size(), 0);

    scheme->pre_stage(1.0);
    EXPECT_TRUE(pre_stage_called);

    scheme->post_stage(1.0, 0, stage_vecs);
    EXPECT_TRUE(post_stage_called);

    scheme->set_cfl_time_local(12.34);
    PetscReal cfl;
    TSGetCFLTime(ts, &cfl);
    EXPECT_DOUBLE_EQ(cfl, 12.34);

    auto U = Vector::create_seq(comm, 3);
    auto y = Vector::create_seq(comm, 3);
    scheme->compute_rhs_function(0., U, y);
    EXPECT_TRUE(compute_rhs_called);

    TSDestroy(&ts);

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("type: test-ts"));
}
