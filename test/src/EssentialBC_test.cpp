#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/Factory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/EssentialBC.h"
#include "godzilla/PiecewiseLinear.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestEssentialBC : public EssentialBC {
public:
    explicit TestEssentialBC(const Parameters & pars) : EssentialBC(pars), components({ 0 }) {}

    MOCK_METHOD(void, evaluate, (Real time, const Real x[], Scalar u[]));
    MOCK_METHOD(void, evaluate_t, (Real time, const Real x[], Scalar u[]));

    const std::vector<Int> &
    get_components() const override
    {
        return this->components;
    }

protected:
    std::vector<Int> components;
};

} // namespace

TEST(EssentialBCTest, api)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = TestEssentialBC::parameters();
    params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &problem)
        .set<std::vector<std::string>>("boundary", {});
    TestEssentialBC bc(params);

    mesh.create();
    problem.create();
    bc.create();

    const auto & comps = bc.get_components();
    EXPECT_EQ(comps.size(), 1);
    EXPECT_EQ(comps[0], 0);
    EXPECT_EQ(bc.get_field_id(), FieldID(0));
}

TEST(EssentialBCTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = TestEssentialBC::parameters();
    params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &problem)
        .set<std::string>("field", "asdf")
        .set<std::vector<std::string>>("boundary", {});
    TestEssentialBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                HasSubstr("Field 'asdf' does not exists. Typo?"));
}

TEST(EssentialBCTest, field_param_not_specified)
{
    testing::internal::CaptureStderr();

    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = TestEssentialBC::parameters();
    params.set<App *>("_app", &app)
        .set<DiscreteProblemInterface *>("_dpi", &problem)
        .set<std::vector<std::string>>("boundary", {});
    TestEssentialBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    EXPECT_FALSE(app.check_integrity());
    app.get_logger()->print();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        HasSubstr(
            "Use the 'field' parameter to assign this boundary condition to an existing field."));
}
