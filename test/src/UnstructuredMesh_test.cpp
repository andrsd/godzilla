#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "UnstructuredMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;

namespace {

class TestUnstructuredMesh : public UnstructuredMesh {
public:
    explicit TestUnstructuredMesh(const Parameters & params) : UnstructuredMesh(params) {}

    void
    create_dm()
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
    }

    Int
    get_partition_overlap()
    {
        return this->partition_overlap;
    }
};

} // namespace

//

TEST(UnstructuredMeshTest, api)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);

    mesh.set_partition_overlap(1);
    mesh.create();

    EXPECT_EQ(mesh.get_num_vertices(), 3);
    EXPECT_EQ(mesh.get_num_elements(), 2);

    Int first, last;
    mesh.get_element_idx_range(first, last);
    EXPECT_EQ(first, 0);
    EXPECT_EQ(last, 2);

    EXPECT_EQ(mesh.get_partition_overlap(), 1);

    EXPECT_TRUE(mesh.is_simplex());
}

TEST(UnstructuredMeshTest, api_ghosted)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);

    mesh.set_partition_overlap(1);
    mesh.create();
    mesh.construct_ghost_cells();

    EXPECT_EQ(mesh.get_num_vertices(), 3);
    EXPECT_EQ(mesh.get_num_elements(), 2);
    EXPECT_EQ(mesh.get_num_all_elements(), 4);

    Int first, last;
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

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    EXPECT_DEATH(mesh.get_face_set_name(1234), "Face set ID '1234' does not exist.");
}

TEST(UnstructuredMeshTest, nonexistent_cell_set)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    EXPECT_DEATH(mesh.get_cell_set_name(1234), "Cell set ID '1234' does not exist.");
}

TEST(UnstructuredMeshTest, get_coordinates)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    Vector coords = mesh.get_coordinates();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);
}

TEST(UnstructuredMeshTest, get_coordinates_local)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    Vector coords = mesh.get_coordinates_local();
    EXPECT_EQ(coords(0), -1.);
    EXPECT_EQ(coords(1), 0.);
    EXPECT_EQ(coords(2), 1.);
}

TEST(UnstructuredMeshTest, get_cell_connectivity)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto cell0 = mesh.get_cell_connectivity(0);
    EXPECT_EQ(cell0[0], 0);
    EXPECT_EQ(cell0[1], 1);
    auto cell1 = mesh.get_cell_connectivity(1);
    EXPECT_EQ(cell1[0], 1);
    EXPECT_EQ(cell1[1], 2);
}

TEST(UnstructuredMeshTest, get_local_section)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    DM dm = mesh.get_dm();

    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[4] = { 1, 0, 0, 0 };
    Section s = Section::create(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL);
    mesh.set_local_section(s);

    Section ls = mesh.get_local_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}

TEST(UnstructuredMeshTest, get_global_section)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    DM dm = mesh.get_dm();

    DMSetNumFields(dm, 1);
    Int nc[1] = { 1 };
    Int n_dofs[4] = { 1, 0, 0, 0 };
    Section s = Section::create(dm, NULL, nc, n_dofs, 0, NULL, NULL, NULL, NULL);
    mesh.set_local_section(s);
    mesh.set_global_section(s);

    Section ls = mesh.get_global_section();

    PetscBool congruent = PETSC_FALSE;
    PetscSectionCompare(s, ls, &congruent);
    EXPECT_EQ(congruent, PETSC_TRUE);
}

TEST(UnstructuredMeshTest, get_num_elem_nodes)
{
    EXPECT_EQ(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_SEGMENT), 2);
    EXPECT_EQ(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_TRIANGLE), 3);
    EXPECT_EQ(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_QUADRILATERAL), 4);
    EXPECT_EQ(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_TETRAHEDRON), 4);
    EXPECT_EQ(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_HEXAHEDRON), 8);

    EXPECT_DEATH(UnstructuredMesh::get_num_elem_nodes(DM_POLYTOPE_PYRAMID), "Unsupported type.");
}
