#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTestFENonlinearProblem.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/RZSymmetry.h"

using namespace godzilla;

TEST(RZSymmetryTest, check_dim)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    // clang-format off
    mesh_pars.set<App *>("_app", &app)
        .set<Int>("nx", 2);
    // clang-format on
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<std::string>("scheme", "rk-2");
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("_app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 0. });
    params.set<std::vector<Real>>("axis", { 1. });
    RZSymmetry rz(params);

    mesh.create();
    rz.create();

    testing::internal::CaptureStderr();
    auto * logger = app.get_logger();
    if (logger->get_num_entries() > 0)
        logger->print();
    auto stdout = testing::internal::GetCapturedStderr();
    EXPECT_THAT(stdout,
                testing::HasSubstr("[ERROR] 'RZSymmetry' can be used only with 2D problems"));
}

TEST(RZSymmetryTest, check_compatible)
{
    TestApp app;

    auto mesh_pars = RectangleMesh::parameters();
    // clang-format off
    mesh_pars.set<App *>("_app", &app)
        .set<Int>("nx", 2)
        .set<Int>("ny", 2);
    // clang-format on
    RectangleMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    prob_pars.set<std::string>("scheme", "rk-2");
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("_app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 0. });
    params.set<std::vector<Real>>("axis", { 1. });
    RZSymmetry rz(params);

    mesh.create();
    rz.create();

    testing::internal::CaptureStderr();
    auto * logger = app.get_logger();
    if (logger->get_num_entries() > 0)
        logger->print();
    auto stdout = testing::internal::GetCapturedStderr();
    EXPECT_THAT(stdout, testing::HasSubstr("[ERROR] 'axis' parameter must provide 2 components."));
    EXPECT_THAT(stdout, testing::HasSubstr("[ERROR] 'point' parameter must provide 2 components."));
}

TEST(RZSymmetryTest, evaluate)
{
    TestApp app;

    auto mesh_pars = LineMesh::parameters();
    mesh_pars.set<App *>("_app", &app);
    mesh_pars.set<Int>("nx", 2);
    LineMesh mesh(mesh_pars);

    auto prob_pars = GTestFENonlinearProblem::parameters();
    prob_pars.set<App *>("_app", &app);
    prob_pars.set<MeshObject *>("_mesh_obj", &mesh);
    GTestFENonlinearProblem prob(prob_pars);

    auto params = RZSymmetry::parameters();
    params.set<App *>("_app", &app);
    params.set<DiscreteProblemInterface *>("_dpi", &prob);
    params.set<std::vector<Real>>("point", { 1., 1. });
    params.set<std::vector<Real>>("axis", { 1., 0. });
    RZSymmetry rz(params);

    mesh.create();
    rz.create();

    DenseVector<Real, 2> coord({ 0., 2. });
    EXPECT_DOUBLE_EQ(rz.get_value(0, coord), 1.);
}
