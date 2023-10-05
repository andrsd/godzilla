#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "UnstructuredMesh.h"
#include "Parameters.h"
#include "petsc.h"

using namespace godzilla;
using namespace testing;

namespace {

IndexSet
points_from_label(const Label & label)
{
    auto ids = label.get_values();
    return label.get_stratum(ids[0]);
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

        PETSC_CHECK(DMPlexCreateBoxMesh(comm(),
                                        1,
                                        PETSC_TRUE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &this->_dm));
    }

    Int
    get_partition_overlap()
    {
        return this->partition_overlap;
    }
};

class TestUnstructuredMesh3D : public UnstructuredMesh {
public:
    explicit TestUnstructuredMesh3D(const Parameters & params) :
        UnstructuredMesh(params),
        nx(get_param<Int>("nx")),
        ny(get_param<Int>("ny")),
        nz(get_param<Int>("nz"))
    {
    }

    void
    create_dm()
    {
        Real lower[3] = { 0, 0, 0 };
        Real upper[3] = { 1, 1, 1 };
        Int faces[3] = { this->nx, this->ny, this->nz };
        DMBoundaryType periodicity[3] = { DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED };

        PETSC_CHECK(DMPlexCreateBoxMesh(comm(),
                                        3,
                                        PETSC_FALSE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &this->_dm));
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

    const Int & nx;
    const Int & ny;
    const Int & nz;
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
    EXPECT_EQ(mesh.get_num_faces(), 3);
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

TEST(UnstructuredMeshTest, get_connectivity)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto conn0 = mesh.get_connectivity(0);
    EXPECT_EQ(conn0[0], 2);
    EXPECT_EQ(conn0[1], 3);
    auto conn1 = mesh.get_connectivity(1);
    EXPECT_EQ(conn1[0], 3);
    EXPECT_EQ(conn1[1], 4);

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
    auto facets = depth_lbl.get_stratum(0);
    facets.get_indices();
    EXPECT_EQ(facets.get_local_size(), 3);
    EXPECT_EQ(facets(0), 2);
    EXPECT_EQ(facets(1), 3);
    EXPECT_EQ(facets(2), 4);
    facets.restore_indices();
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

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.set_partition_overlap(1);
    mesh.create();
    mesh.construct_ghost_cells();

    EXPECT_EQ(*mesh.vertex_begin(), 12);
    EXPECT_EQ(*mesh.vertex_end(), 24);

    auto vtx_range = mesh.get_vertex_range();
    EXPECT_EQ(vtx_range.first(), 12);
    EXPECT_EQ(vtx_range.last(), 24);

    EXPECT_EQ(*mesh.face_begin(), 24);
    EXPECT_EQ(*mesh.face_end(), 35);

    auto face_range = mesh.get_face_range();
    EXPECT_EQ(face_range.first(), 24);
    EXPECT_EQ(face_range.last(), 35);

    EXPECT_EQ(*mesh.cell_begin(), 0);
    EXPECT_EQ(*mesh.cell_end(), 12);

    auto cell_range = mesh.get_cell_range();
    EXPECT_EQ(cell_range.first(), 0);
    EXPECT_EQ(cell_range.last(), 2);

    auto all_cell_range = mesh.get_all_cell_range();
    EXPECT_EQ(all_cell_range.first(), 0);
    EXPECT_EQ(all_cell_range.last(), 12);

    auto it = mesh.cell_begin();
    for (Int i = 0; i < 12; i++)
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
    params.set<Int>("nx") = 1;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
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

TEST(UnstructuredMesh, compute_cell_geometry)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    {
        Real vol;
        Real centroid[3];
        mesh.compute_cell_geometry(0, &vol, centroid, nullptr);
        EXPECT_DOUBLE_EQ(centroid[0], 0.25);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);
    }
    {
        Real vol;
        Real centroid[3];
        mesh.compute_cell_geometry(1, &vol, centroid, nullptr);
        EXPECT_DOUBLE_EQ(centroid[0], 0.75);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);
    }

    {
        Real vol;
        Real centroid[3];
        Real normal[3];
        mesh.compute_cell_geometry(14, &vol, centroid, normal);
        EXPECT_DOUBLE_EQ(centroid[0], 0.);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(normal[0], 1.);
        EXPECT_DOUBLE_EQ(normal[1], 0.);
        EXPECT_DOUBLE_EQ(normal[2], 0.);
    }
    {
        Real vol;
        Real centroid[3];
        Real normal[3];
        mesh.compute_cell_geometry(16, &vol, centroid, normal);
        EXPECT_DOUBLE_EQ(centroid[0], 1.);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(normal[0], 1.);
        EXPECT_DOUBLE_EQ(normal[1], 0.);
        EXPECT_DOUBLE_EQ(normal[2], 0.);
    }
}

TEST(UnstructuredMesh, get_face_set_label_nonexistent)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto nonex_lbl = mesh.get_face_set_label("asdf");
    EXPECT_EQ((DMLabel) nonex_lbl, nullptr);

    auto front_lbl = mesh.get_face_set_label("front");
    EXPECT_EQ((DMLabel) front_lbl, (DMLabel) mesh.get_label("front"));
}

TEST(UnstructuredMesh, get_chart)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    Int start, end;
    mesh.get_chart(start, end);
    EXPECT_EQ(start, 0);
    EXPECT_EQ(end, 45);

    auto range = mesh.get_chart();
    EXPECT_EQ(range.first(), 0);
    EXPECT_EQ(range.last(), 45);
}

TEST(UnstructuredMesh, common_cells_by_vertex)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<const App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto map = mesh.common_cells_by_vertex();
    EXPECT_EQ(map.size(), 12);
    EXPECT_THAT(map[2], UnorderedElementsAre(0));
    EXPECT_THAT(map[3], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map[4], UnorderedElementsAre(1));
    EXPECT_THAT(map[5], UnorderedElementsAre(0));
    EXPECT_THAT(map[6], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map[7], UnorderedElementsAre(1));
    EXPECT_THAT(map[8], UnorderedElementsAre(0));
    EXPECT_THAT(map[9], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map[10], UnorderedElementsAre(1));
    EXPECT_THAT(map[11], UnorderedElementsAre(0));
    EXPECT_THAT(map[12], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map[13], UnorderedElementsAre(1));

    // subsequent calls to `common_cells_by_vertex()` must give back the same map
    auto map1 = mesh.common_cells_by_vertex();
    EXPECT_EQ(map1.size(), 12);
    EXPECT_THAT(map1[2], UnorderedElementsAre(0));
    EXPECT_THAT(map1[3], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map1[4], UnorderedElementsAre(1));
    EXPECT_THAT(map1[5], UnorderedElementsAre(0));
    EXPECT_THAT(map1[6], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map1[7], UnorderedElementsAre(1));
    EXPECT_THAT(map1[8], UnorderedElementsAre(0));
    EXPECT_THAT(map1[9], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map1[10], UnorderedElementsAre(1));
    EXPECT_THAT(map1[11], UnorderedElementsAre(0));
    EXPECT_THAT(map1[12], UnorderedElementsAre(0, 1));
    EXPECT_THAT(map1[13], UnorderedElementsAre(1));
}

TEST(UnstructuredMesh, build_from_cell_list_2d)
{
    class TestMesh2D : public godzilla::UnstructuredMesh {
    public:
        explicit TestMesh2D(const godzilla::Parameters & parameters) : UnstructuredMesh(parameters)
        {
        }

    protected:
        void
        create_dm() override
        {
            std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
            std::vector<Real> vertices = { 0, 0, 1, 0, 0, 1, 1, 1 };
            build_from_cell_list(2, 3, cells, 2, vertices, true);
        }
    };

    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<const godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();

    EXPECT_EQ(mesh.get_num_cells(), 2);
    EXPECT_THAT(mesh.get_connectivity(0), ElementsAre(2, 3, 4));
    EXPECT_THAT(mesh.get_connectivity(1), ElementsAre(3, 5, 4));
    EXPECT_EQ(mesh.get_num_vertices(), 4);
    auto coords = mesh.get_coordinates();
    EXPECT_EQ(coords.get_size(), 8);
    EXPECT_DOUBLE_EQ(coords(0), 0.);
    EXPECT_DOUBLE_EQ(coords(1), 0.);
    EXPECT_DOUBLE_EQ(coords(2), 1.);
    EXPECT_DOUBLE_EQ(coords(3), 0.);
    EXPECT_DOUBLE_EQ(coords(4), 0.);
    EXPECT_DOUBLE_EQ(coords(5), 1.);
    EXPECT_DOUBLE_EQ(coords(6), 1.);
    EXPECT_DOUBLE_EQ(coords(7), 1.);
}
