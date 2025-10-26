#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Parameters.h"
#include "petscdmplex.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestMesh : public Object {
public:
    explicit TestMesh(const Parameters & pars) : Object(pars) {}

    Qtr<UnstructuredMesh>
    create_mesh()
    {
        Real lower[1] = { -1 };
        Real upper[1] = { 1 };
        Int faces[1] = { 2 };
        DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

        DM dm;
#if PETSC_VERSION_GE(3, 22, 0)
        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        1,
                                        PETSC_TRUE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        0,
                                        PETSC_FALSE,
                                        &dm));
#else
        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        1,
                                        PETSC_TRUE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &dm));
#endif
        return Qtr<UnstructuredMesh>::alloc(dm);
    }
};

} // namespace

TEST(MeshTest, get_coordinates)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    Vector coords = mesh->get_coordinates();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);

    DM cdm;
    DMGetCoordinateDM(mesh->get_dm(), &cdm);
    EXPECT_EQ(mesh->get_coordinate_dm(), cdm);
}

TEST(MeshTest, get_coordinates_local)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    Vector coords = mesh->get_coordinates_local();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);
}

TEST(MeshTest, get_coordinates_section)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    auto section = mesh->get_coordinate_section();
    EXPECT_EQ(section.get_offset(2), 0);
    EXPECT_EQ(section.get_offset(3), 1);
    EXPECT_EQ(section.get_offset(4), 2);
}

TEST(MeshTest, remove_label)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    mesh->remove_label("marker");
    EXPECT_FALSE(mesh->has_label("marker"));
}

TEST(MeshTest, set_label_value)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    mesh->create_label("bnd");
    auto bnd = mesh->get_label("bnd");
    mesh->set_label_value("bnd", 0, 1001);

    EXPECT_EQ(bnd.get_value(0), 1001);
}

TEST(MeshTest, clear_label_value)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    mesh->create_label("bnd");
    auto bnd = mesh->get_label("bnd");
    mesh->set_label_value("bnd", 0, 1001);
    mesh->clear_label_value("bnd", 0, 1001);
    EXPECT_EQ(bnd.get_value(0), -1);
}

TEST(MeshTest, view)
{
    testing::internal::CaptureStdout();

    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    mesh->view();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("DM Object:"));
    EXPECT_THAT(out, HasSubstr("type: plex"));
}

TEST(MeshTest, get_neighbors)
{
    TestApp app;

    auto params = TestMesh::parameters();
    params.set<App *>("app", &app);
    params.set<std::string>("name", "obj");
    auto mesh_qtr = MeshFactory::create<TestMesh>(params);
    auto mesh = mesh_qtr.get();

    if (app.get_comm().size() == 1) {
        auto neighbors = mesh->get_neighbors();
        EXPECT_TRUE(neighbors.empty());
    }
}
