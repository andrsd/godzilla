#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshObject.h"
#include "godzilla/Parameters.h"
#include "petscdmplex.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestMesh : public MeshObject {
public:
    explicit TestMesh(const Parameters & params) : MeshObject(params) {}

    Mesh *
    create_mesh() override
    {
        Real lower[1] = { -1 };
        Real upper[1] = { 1 };
        Int faces[1] = { 2 };
        DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

        DM dm;
        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        1,
                                        PETSC_TRUE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &dm));
        return new UnstructuredMesh(dm);
    }
};

} // namespace

TEST(MeshTest, get_coordinates)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    Vector coords = m->get_coordinates();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);

    DM cdm;
    DMGetCoordinateDM(m->get_dm(), &cdm);
    EXPECT_EQ(m->get_coordinate_dm(), cdm);
}

TEST(MeshTest, get_coordinates_local)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    Vector coords = m->get_coordinates_local();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);
}

TEST(MeshTest, get_coordinates_section)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto section = m->get_coordinate_section();
    EXPECT_EQ(section.get_offset(2), 0);
    EXPECT_EQ(section.get_offset(3), 1);
    EXPECT_EQ(section.get_offset(4), 2);
}

TEST(MeshTest, remove_label)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->remove_label("marker");
    EXPECT_FALSE(m->has_label("marker"));
}

TEST(MeshTest, set_label_value)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->create_label("bnd");
    auto bnd = m->get_label("bnd");
    m->set_label_value("bnd", 0, 1001);

    EXPECT_EQ(bnd.get_value(0), 1001);
}

TEST(MeshTest, clear_label_value)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->create_label("bnd");
    auto bnd = m->get_label("bnd");
    m->set_label_value("bnd", 0, 1001);
    m->clear_label_value("bnd", 0, 1001);
    EXPECT_EQ(bnd.get_value(0), -1);
}

TEST(MeshTest, view)
{
    testing::internal::CaptureStdout();

    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    auto m = mesh.create_mesh();
    m->view();

    auto out = testing::internal::GetCapturedStdout();
    EXPECT_THAT(out, HasSubstr("DM Object:"));
    EXPECT_THAT(out, HasSubstr("type: plex"));
}
