#include "gmock/gmock.h"
#include "TestApp.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/L2Diff.h"

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

} // namespace

TEST(L2DiffTest, DISABLED_compute)
{
    TestApp app;

    auto mesh_params = LineMesh::parameters();
    mesh_params.set<App *>("_app", &app);
    mesh_params.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_params);

    auto prob_params = GTestFENonlinearProblem::parameters();
    prob_params.set<App *>("_app", &app);
    prob_params.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_params);
    app.set_problem(&prob);

    auto bc_left_params = DirichletBC::parameters();
    bc_left_params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "left" });
    DirichletBC bc_left(bc_left_params);

    auto bc_right_params = DirichletBC::parameters();
    bc_right_params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &prob)
        .set<std::vector<std::string>>("boundary", { "right" });
    DirichletBC bc_right(bc_right_params);

    auto ps_params = L2Diff::parameters();
    ps_params.set<App *>("_app", &app);
    ps_params.set<Problem *>("_problem", &prob);
    ps_params.set<std::vector<std::string>>("value", { "x*x" });
    L2Diff ps(ps_params);

    prob.add_boundary_condition(&bc_left);
    prob.add_boundary_condition(&bc_right);
    prob.add_postprocessor(&ps);

    prob.create();

    prob.run();
    prob.compute_postprocessors();

    auto l2_err = ps.get_value();
    ASSERT_TRUE(!l2_err.empty());
    EXPECT_NEAR(l2_err[0], 0.0416667, 1e-7);
}
