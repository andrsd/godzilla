#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "GodzillaApp_test.h"
#include "UnstructuredMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;
using namespace testing;

namespace {

IndexSet
points_from_label(const DMLabel & label)
{
    IndexSet is = IndexSet::values_from_label(label);
    is.get_indices();
    auto ids = is.to_std_vector();
    is.restore_indices();
    is.destroy();

    return IndexSet::stratum_from_label(label, ids[0]);
}

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

class TestUnstructuredMesh3D : public UnstructuredMesh {
public:
    explicit TestUnstructuredMesh3D(const Parameters & params) : UnstructuredMesh(params) {}

    void
    create_dm()
    {
        Real lower[3] = { 0, 0, 0 };
        Real upper[3] = { 1, 1, 1 };
        Int faces[3] = { 1, 1, 1 };
        DMBoundaryType periodicity[3] = { DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED };

        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        3,
                                        PETSC_FALSE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &this->dm));
        // create "side sets"
        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "back";
        face_set_names[2] = "front";
        face_set_names[3] = "bottom";
        face_set_names[4] = "top";
        face_set_names[5] = "right";
        face_set_names[6] = "left";
        create_face_set_labels(face_set_names);
        for (auto it : face_set_names)
            set_face_set_name(it.first, it.second);
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
    EXPECT_EQ(mesh.get_num_cells(), 2);

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
    EXPECT_EQ(mesh.get_num_cells(), 2);
    EXPECT_EQ(mesh.get_num_all_cells(), 4);

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

    DM cdm;
    DMGetCoordinateDM(mesh.get_dm(), &cdm);
    EXPECT_EQ(mesh.get_coordinate_dm(), cdm);
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

    auto support2 = mesh.get_support(2);
    EXPECT_EQ(support2.size(), 1);
    EXPECT_EQ(support2[0], 0);

    auto support3 = mesh.get_support(3);
    EXPECT_EQ(support3.size(), 2);
    EXPECT_EQ(support3[0], 0);
    EXPECT_EQ(support3[1], 1);

    auto support4 = mesh.get_support(4);
    EXPECT_EQ(support4.size(), 1);
    EXPECT_EQ(support4[0], 1);

    auto cone0 = mesh.get_cone(0);
    EXPECT_EQ(cone0.size(), 2);
    EXPECT_EQ(cone0[0], 2);
    EXPECT_EQ(cone0[1], 3);

    auto cone1 = mesh.get_cone(1);
    EXPECT_EQ(cone1.size(), 2);
    EXPECT_EQ(cone1[0], 3);
    EXPECT_EQ(cone1[1], 4);

    auto depth_lbl = mesh.get_depth_label();
    IndexSet facets = IndexSet::stratum_from_label(depth_lbl, 0);
    facets.get_indices();
    EXPECT_EQ(facets.get_local_size(), 3);
    EXPECT_EQ(facets(0), 2);
    EXPECT_EQ(facets(1), 3);
    EXPECT_EQ(facets(2), 4);
    facets.restore_indices();
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

TEST(UnstructuredMeshTest, get_num_cell_nodes)
{
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_POINT), 1);
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_SEGMENT), 2);
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_TRIANGLE), 3);
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_QUADRILATERAL), 4);
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_TETRAHEDRON), 4);
    EXPECT_EQ(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_HEXAHEDRON), 8);

    EXPECT_DEATH(UnstructuredMesh::get_num_cell_nodes(DM_POLYTOPE_PYRAMID), "Unsupported type.");
}

TEST(UnstructuredMeshTest, ranges)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.set_partition_overlap(1);
    mesh.create();
    mesh.construct_ghost_cells();

    EXPECT_EQ(*mesh.vertex_begin(), 4);
    EXPECT_EQ(*mesh.vertex_end(), 7);

    auto vtx_range = mesh.get_vertex_range();
    EXPECT_EQ(vtx_range.get_first(), 4);
    EXPECT_EQ(vtx_range.get_last(), 7);

    EXPECT_EQ(*mesh.cell_begin(), 0);
    EXPECT_EQ(*mesh.cell_end(), 4);

    auto cell_range = mesh.get_cell_range();
    EXPECT_EQ(cell_range.get_first(), 0);
    EXPECT_EQ(cell_range.get_last(), 2);

    auto all_cell_range = mesh.get_all_cell_range();
    EXPECT_EQ(all_cell_range.get_first(), 0);
    EXPECT_EQ(all_cell_range.get_last(), 4);

    auto it = mesh.cell_begin();
    for (Int i = 0; i < 4; i++)
        it++;
    EXPECT_TRUE(it == mesh.cell_end());
}

TEST(UnstructuredMeshTest, polytope_type_str)
{
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_POINT), "POINT");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_SEGMENT), "SEGMENT");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_POINT_PRISM_TENSOR), "POINT_PRISM_TENSOR");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_TRIANGLE), "TRIANGLE");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_QUADRILATERAL), "QUADRILATERAL");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_SEG_PRISM_TENSOR), "SEG_PRISM_TENSOR");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_TETRAHEDRON), "TETRAHEDRON");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_HEXAHEDRON), "HEXAHEDRON");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_TRI_PRISM), "TRI_PRISM");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_TRI_PRISM_TENSOR), "TRI_PRISM_TENSOR");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_QUAD_PRISM_TENSOR), "QUAD_PRISM_TENSOR");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_PYRAMID), "PYRAMID");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_FV_GHOST), "FV_GHOST");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_INTERIOR_GHOST), "INTERIOR_GHOST");
    EXPECT_STREQ(get_polytope_type_str(DM_POLYTOPE_UNKNOWN), "UNKNOWN");
}

TEST(UnstructuredMesh, get_cone_recursive_vertices)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto depth_label = mesh.get_depth_label();
    auto left = mesh.get_label("left");
    IndexSet left_facet = points_from_label(left);
    IndexSet left_points = mesh.get_cone_recursive_vertices(left_facet);
    left_points.sort_remove_dups();
    left_points.get_indices();
    auto idxs = left_points.to_std_vector();
    EXPECT_THAT(idxs, ::UnorderedElementsAre(1, 3, 5, 7));
    left_points.restore_indices();
}
