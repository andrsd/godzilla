#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "UnstructuredMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;

class TestUnstructuredMesh : public UnstructuredMesh {
public:
    explicit TestUnstructuredMesh(const InputParameters & params) : UnstructuredMesh(params) {}

    void
    create_dm()
    {
        PetscReal lower[1] = { -1 };
        PetscReal upper[1] = { 1 };
        PetscInt faces[1] = { 2 };
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
    }

    PetscInt
    get_partition_overlap()
    {
        return this->partition_overlap;
    }
};

//

TEST(UnstructuredMeshTest, api)
{
    TestApp app;

    InputParameters params = TestUnstructuredMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);

    mesh.set_partition_overlap(1);
    mesh.create();

    EXPECT_EQ(mesh.get_num_vertices(), 3);
    EXPECT_EQ(mesh.get_num_elements(), 2);

    PetscInt first, last;
    mesh.get_element_idx_range(first, last);
    EXPECT_EQ(first, 0);
    EXPECT_EQ(last, 2);

    EXPECT_EQ(mesh.get_partition_overlap(), 1);

    EXPECT_TRUE(mesh.is_simplex());
}

TEST(UnstructuredMeshTest, api_ghosted)
{
    TestApp app;

    InputParameters params = TestUnstructuredMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);

    mesh.set_partition_overlap(1);
    mesh.create();
    mesh.construct_ghost_cells();

    EXPECT_EQ(mesh.get_num_vertices(), 3);
    EXPECT_EQ(mesh.get_num_elements(), 2);
    EXPECT_EQ(mesh.get_num_all_elements(), 4);

    PetscInt first, last;
    mesh.get_element_idx_range(first, last);
    EXPECT_EQ(first, 0);
    EXPECT_EQ(last, 2);
    mesh.get_all_element_idx_range(first, last);
    EXPECT_EQ(first, 0);
    EXPECT_EQ(last, 4);

    EXPECT_EQ(mesh.get_partition_overlap(), 1);

    EXPECT_TRUE(mesh.is_simplex());
}

TEST(UnstructuredMeshTest, nonexistent_face_set)
{
    TestApp app;

    InputParameters params = TestUnstructuredMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    EXPECT_DEATH(mesh.get_face_set_name(1234), "Face set ID '1234' does not exist.");
}

TEST(UnstructuredMeshTest, nonexistent_cell_set)
{
    TestApp app;

    InputParameters params = TestUnstructuredMesh::valid_params();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    EXPECT_DEATH(mesh.get_cell_set_name(1234), "Cell set ID '1234' does not exist.");
}
