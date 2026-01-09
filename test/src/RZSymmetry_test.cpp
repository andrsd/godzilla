#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/RZSymmetry.h"

using namespace godzilla;

TEST(RZSymmetryTest, check_dim)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    // clang-format off
    mesh_pars.set<App *>("app", &app)
        .set<Int>("nx", 2);
    // clang-format on
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 0. });
    params.set<std::vector<Real>>("axis", { 1. });
    RZSymmetry rz(params);

    EXPECT_DEATH(rz.create(), "'RZSymmetry' can be used only with 2D problems");
}

TEST(RZSymmetryTest, check_compatible)
{
    TestApp app;

    auto mesh_pars = RectangleMesh::parameters();
    // clang-format off
    mesh_pars.set<App *>("app", &app)
        .set<Int>("nx", 2)
        .set<Int>("ny", 2);
    // clang-format on
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 0. });
    params.set<std::vector<Real>>("axis", { 1. });
    RZSymmetry rz(params);

    EXPECT_DEATH(rz.create(), "'axis' parameter must provide 2 components.");
}

TEST(RZSymmetryTest, evaluate)
{
    TestApp app;

    auto mesh_pars = RectangleMesh::parameters();
    // clang-format off
    mesh_pars.set<App *>("app", &app)
        .set<Int>("nx", 2)
        .set<Int>("ny", 2);
    // clang-format on
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("app", &app);
    prob_pars.set<Mesh *>("mesh", mesh.get());
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 1., 1. });
    params.set<std::vector<Real>>("axis", { 1., 0. });
    RZSymmetry rz(params);

    rz.create();

    DenseVector<Real, 2> coord({ 0., 2. });
    EXPECT_DOUBLE_EQ(rz.get_value(0, coord), 1.);
}
