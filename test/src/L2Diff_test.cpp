#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/L2Diff.h"

using namespace godzilla;

namespace {

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = x[0] * x[0];
    }
};

class L2Error : public L2Diff {
public:
    explicit L2Error(const Parameters & pars) : L2Diff(pars) {}

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = x[0] * x[0];
    }
};

} // namespace

TEST(L2DiffTest, compute)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<Ref<App>>("app", ref(app));
    mesh_params.set<Int>("nx", 20);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = app.make_parameters<GTestFENonlinearProblem>();
    prob_params.set<Ref<Mesh>>("mesh", ref(*mesh));
    auto prob = app.make_problem<GTestFENonlinearProblem>(prob_params);

    auto bc_left_params = DirichletBC::parameters();
    bc_left_params.set<Ref<App>>("app", ref(app));
    bc_left_params.set<std::vector<String>>("boundary", { "left" });
    prob->add_boundary_condition<DirichletBC>(bc_left_params);

    auto bc_right_params = DirichletBC::parameters();
    bc_right_params.set<Ref<App>>("app", ref(app));
    bc_right_params.set<std::vector<String>>("boundary", { "right" });
    prob->add_boundary_condition<DirichletBC>(bc_right_params);

    auto ps_params = L2Error::parameters();
    ps_params.set<Ref<App>>("app", ref(app));
    auto ps = prob->add_postprocessor<L2Error>(ps_params);

    prob->create();

    prob->run();
    prob->compute_postprocessors();

    auto l2_err = ps->get_value();
    ASSERT_TRUE(!l2_err.empty());
    EXPECT_NEAR(l2_err[0], 0.000416667, 1e-7);
}
