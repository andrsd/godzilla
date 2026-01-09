#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/EssentialBC.h"

using namespace godzilla;
using namespace testing;

namespace {

class DirichletBC : public EssentialBC {
public:
    explicit DirichletBC(const Parameters & pars) : EssentialBC(pars) {}

    void
    evaluate(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = time * (x[0] + x[1] + x[2]);
    }

    void
    evaluate_t(Real time, const Real x[], Scalar u[]) override
    {
        u[0] = 1.;
    }

    std::vector<Int>
    create_components() override
    {
        return { 0 };
    }
};

} // namespace

TEST(EssentialBCTest, test)
{
    TestApp app;

    auto mesh_pars = BoxMesh::parameters();
    // clang-format off
    mesh_pars
        .set<App *>("app", &app)
        .set<Int>("nx", 2)
        .set<Int>("ny", 2)
        .set<Int>("nz", 2);
    // clang-format on
    auto mesh = MeshFactory::create<BoxMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = DirichletBC::parameters();
    params.set<App *>("app", &app);
    params.set<std::vector<String>>("boundary", {});
    auto bc = problem.add_boundary_condition<DirichletBC>(params);

    problem.create();

    const auto & components = bc->get_components();
    ASSERT_EQ(components.size(), 1);
    EXPECT_THAT(components, testing::ElementsAre(0));
    EXPECT_EQ(bc->get_field_id(), FieldID(0));

    Real time = 2.5;
    Real x[] = { 3, 5, 7 };
    Scalar u[] = { 0 };

    bc->evaluate(time, x, u);
    EXPECT_EQ(u[0], 37.5);

    bc->evaluate_t(time, x, u);
    EXPECT_EQ(u[0], 1.);
}

TEST(EssentialBCTest, non_existing_field)
{
    TestApp app;

    Parameters mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    Parameters prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    Parameters params = DirichletBC::parameters();
    params.set<App *>("app", &app)
        .set<String>("field", "asdf")
        .set<std::vector<String>>("boundary", {});
    problem.add_boundary_condition<DirichletBC>(params);

    EXPECT_DEATH(problem.create(), "Field 'asdf' does not exists. Typo?");
}

TEST(EssentialBCTest, field_param_not_specified)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("app", &app);
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTest2FieldsFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTest2FieldsFENonlinearProblem problem(prob_pars);
    app.set_problem(&problem);

    auto params = DirichletBC::parameters();
    params.set<App *>("app", &app);
    params.set<std::vector<String>>("boundary", {});
    problem.add_boundary_condition<DirichletBC>(params);

    EXPECT_DEATH(
        problem.create(),
        "Use the 'field' parameter to assign this boundary condition to an existing field.");
}
