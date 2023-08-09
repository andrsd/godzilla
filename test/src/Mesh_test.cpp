#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "UnstructuredMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;
using namespace testing;

namespace {

class TestMesh : public Mesh {
public:
    explicit TestMesh(const Parameters & params) : Mesh(params) {}

    void
    create() override
    {
        Real lower[1] = { -1 };
        Real upper[1] = { 1 };
        Int faces[1] = { 2 };
        DMBoundaryType periodicity[1] = { DM_BOUNDARY_GHOSTED };

        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        1,
                                        PETSC_TRUE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &this->dm));
        PETSC_CHECK(DMSetUp(this->dm));
    }

    void
    distribute() override
    {
    }
};

} // namespace

TEST(MeshTest, get_coordinates)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    Vector coords = mesh.get_coordinates();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);

    DM cdm;
    DMGetCoordinateDM(mesh.get_dm(), &cdm);
    EXPECT_EQ(mesh.get_coordinate_dm(), cdm);
}

TEST(MeshTest, get_coordinates_local)
{
    TestApp app;

    Parameters params = TestMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestMesh mesh(params);
    mesh.create();

    Vector coords = mesh.get_coordinates_local();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);
}