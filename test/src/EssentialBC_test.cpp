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

    MOCK_METHOD(void, evaluate, (Int dim, Real time, const Real x[], Int nc, Scalar u[]));
    MOCK_METHOD(void, evaluate_t, (Int dim, Real time, const Real x[], Int nc, Scalar u[]));

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

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = TestEssentialBC::parameters();
    params.set<App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    TestEssentialBC bc(params);

    mesh.create();
    problem.create();
    bc.create();

    const auto & comps = bc.get_components();
    EXPECT_EQ(comps.size(), 1);
    EXPECT_EQ(comps[0], 0);

    Real x[] = { 3 };
    Scalar u[] = { 0 };

    EXPECT_CALL(bc, evaluate);
    PetscFunc * fn = bc.get_function();
    (*fn)(1, 0, x, 1, u, &bc);

    EXPECT_CALL(bc, evaluate_t);
    PetscFunc * fn_t = bc.get_function_t();
    (*fn_t)(1, 0, x, 1, u, &bc);

    EXPECT_EQ(bc.get_context(), &bc);
    EXPECT_EQ(bc.get_field_id(), 0);
}

TEST(EssentialBCTest, non_existing_field)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = TestEssentialBC::parameters();
    params.set<App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    params.set<std::string>("field") = "asdf";
    TestEssentialBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    app.check_integrity();

    EXPECT_THAT(testing::internal::GetCapturedStderr(),
                HasSubstr("Field 'asdf' does not exists. Typo?"));
}

TEST(EssentialBCTest, field_param_not_specified)
{
    testing::internal::CaptureStderr();

    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app") = &app;
    mesh_pars.set<Int>("nx") = 2;
    LineMesh mesh(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app") = &app;
    prob_pars.set<MeshObject *>("_mesh_obj") = &mesh;
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = TestEssentialBC::parameters();
    params.set<App *>("_app") = &app;
    params.set<DiscreteProblemInterface *>("_dpi") = &problem;
    TestEssentialBC bc(params);

    mesh.create();
    problem.add_boundary_condition(&bc);
    problem.create();

    app.check_integrity();

    EXPECT_THAT(
        testing::internal::GetCapturedStderr(),
        HasSubstr(
            "Use the 'field' parameter to assign this boundary condition to an existing field."));
}
