#include "gmock/gmock.h"
#include "GodzillaApp_test.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshObject.h"
#include "godzilla/Parameters.h"
#include "ExceptionTestMacros.h"
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

class TestUnstructuredMesh : public MeshObject {
public:
    explicit TestUnstructuredMesh(const Parameters & params) : MeshObject(params) {}

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

class TestUnstructuredMesh3D : public MeshObject {
public:
    explicit TestUnstructuredMesh3D(const Parameters & params) :
        MeshObject(params),
        nx(get_param<Int>("nx")),
        ny(get_param<Int>("ny")),
        nz(get_param<Int>("nz"))
    {
    }

    Mesh *
    create_mesh() override
    {
        Real lower[3] = { 0, 0, 0 };
        Real upper[3] = { 1, 1, 1 };
        Int faces[3] = { this->nx, this->ny, this->nz };
        DMBoundaryType periodicity[3] = { DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED,
                                          DM_BOUNDARY_GHOSTED };

        DM dm;
        PETSC_CHECK(DMPlexCreateBoxMesh(get_comm(),
                                        3,
                                        PETSC_FALSE,
                                        faces,
                                        lower,
                                        upper,
                                        periodicity,
                                        PETSC_TRUE,
                                        &dm));
        auto m = new UnstructuredMesh(dm);

        // create "side sets"
        std::map<Int, std::string> face_set_names;
        face_set_names[1] = "back";
        face_set_names[2] = "front";
        face_set_names[3] = "bottom";
        face_set_names[4] = "top";
        face_set_names[5] = "right";
        face_set_names[6] = "left";
        m->create_face_set_labels(face_set_names);
        for (auto it : face_set_names)
            m->set_face_set_name(it.first, it.second);

        return m;
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
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);

    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();

    EXPECT_EQ(m->get_num_vertices(), 3);
    EXPECT_EQ(m->get_num_faces(), 3);
    EXPECT_EQ(m->get_num_cells(), 2);

    EXPECT_TRUE(m->is_simplex());
}

TEST(UnstructuredMeshTest, api_ghosted)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->construct_ghost_cells();

    EXPECT_EQ(m->get_num_vertices(), 3);
    EXPECT_EQ(m->get_num_cells(), 2);
    EXPECT_EQ(m->get_num_all_cells(), 4);

    EXPECT_TRUE(m->is_simplex());

    auto ghost_range = m->get_ghost_cell_range();
    EXPECT_EQ(ghost_range.first(), 2);
    EXPECT_EQ(ghost_range.last(), 4);
}

TEST(UnstructuredMeshTest, nonexistent_face_set)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    EXPECT_THROW_MSG(m->get_face_set_name(1234), "Face set ID '1234' does not exist.");
}

TEST(UnstructuredMeshTest, nonexistent_cell_set)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    EXPECT_THROW_MSG(m->get_cell_set_name(1234), "Cell set ID '1234' does not exist.");
}

TEST(UnstructuredMeshTest, get_connectivity)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto conn0 = m->get_connectivity(0);
    EXPECT_EQ(conn0[0], 2);
    EXPECT_EQ(conn0[1], 3);
    auto conn1 = m->get_connectivity(1);
    EXPECT_EQ(conn1[0], 3);
    EXPECT_EQ(conn1[1], 4);

    auto support2 = m->get_support(2);
    EXPECT_EQ(support2.size(), 1);
    EXPECT_EQ(support2[0], 0);

    auto support3 = m->get_support(3);
    EXPECT_EQ(support3.size(), 2);
    EXPECT_EQ(support3[0], 0);
    EXPECT_EQ(support3[1], 1);

    auto support4 = m->get_support(4);
    EXPECT_EQ(support4.size(), 1);
    EXPECT_EQ(support4[0], 1);

    auto cone0 = m->get_cone(0);
    EXPECT_EQ(cone0.size(), 2);
    EXPECT_EQ(cone0[0], 2);
    EXPECT_EQ(cone0[1], 3);

    auto cone1 = m->get_cone(1);
    EXPECT_EQ(cone1.size(), 2);
    EXPECT_EQ(cone1[0], 3);
    EXPECT_EQ(cone1[1], 4);

    auto depth_lbl = m->get_depth_label();
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
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    m->construct_ghost_cells();

    EXPECT_EQ(*m->vertex_begin(), 12);
    EXPECT_EQ(*m->vertex_end(), 24);

    auto vtx_range = m->get_vertex_range();
    EXPECT_EQ(vtx_range.first(), 12);
    EXPECT_EQ(vtx_range.last(), 24);

    EXPECT_EQ(*m->face_begin(), 24);
    EXPECT_EQ(*m->face_end(), 35);

    auto face_range = m->get_face_range();
    EXPECT_EQ(face_range.first(), 24);
    EXPECT_EQ(face_range.last(), 35);

    EXPECT_EQ(*m->cell_begin(), 0);
    EXPECT_EQ(*m->cell_end(), 12);

    auto cell_range = m->get_cell_range();
    EXPECT_EQ(cell_range.first(), 0);
    EXPECT_EQ(cell_range.last(), 2);

    auto all_cell_range = m->get_all_cell_range();
    EXPECT_EQ(all_cell_range.first(), 0);
    EXPECT_EQ(all_cell_range.last(), 12);

    auto it = m->cell_begin();
    for (Int i = 0; i < 12; i++)
        it++;
    EXPECT_TRUE(it == m->cell_end());
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
    params.set<App *>("_app") = &app;
    params.set<Int>("nx") = 1;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    auto depth_label = m->get_depth_label();
    auto left = m->get_label("left");
    IndexSet left_facet = points_from_label(left);
    IndexSet left_points = m->get_cone_recursive_vertices(left_facet);
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
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    {
        Real vol;
        Real centroid[3];
        m->compute_cell_geometry(0, &vol, centroid, nullptr);
        EXPECT_DOUBLE_EQ(centroid[0], 0.25);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(m->compute_cell_volume(0), 0.5);
    }
    {
        Real vol;
        Real centroid[3];
        m->compute_cell_geometry(1, &vol, centroid, nullptr);
        EXPECT_DOUBLE_EQ(centroid[0], 0.75);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(m->compute_cell_volume(1), 0.5);
    }

    {
        Real vol;
        Real centroid[3];
        Real normal[3];
        m->compute_cell_geometry(14, &vol, centroid, normal);
        EXPECT_DOUBLE_EQ(centroid[0], 0.);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(normal[0], 1.);
        EXPECT_DOUBLE_EQ(normal[1], 0.);
        EXPECT_DOUBLE_EQ(normal[2], 0.);

        EXPECT_DOUBLE_EQ(m->compute_cell_volume(14), 1.);
    }
    {
        Real vol;
        Real centroid[3];
        Real normal[3];
        m->compute_cell_geometry(16, &vol, centroid, normal);
        EXPECT_DOUBLE_EQ(centroid[0], 1.);
        EXPECT_DOUBLE_EQ(centroid[1], 0.5);
        EXPECT_DOUBLE_EQ(centroid[2], 0.5);

        EXPECT_DOUBLE_EQ(normal[0], 1.);
        EXPECT_DOUBLE_EQ(normal[1], 0.);
        EXPECT_DOUBLE_EQ(normal[2], 0.);

        EXPECT_DOUBLE_EQ(m->compute_cell_volume(16), 1.);
    }
}

TEST(UnstructuredMesh, get_face_set_label_nonexistent)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    auto nonex_lbl = m->get_face_set_label("asdf");
    EXPECT_EQ((DMLabel) nonex_lbl, nullptr);

    auto front_lbl = m->get_face_set_label("front");
    EXPECT_EQ((DMLabel) front_lbl, (DMLabel) m->get_label("front"));
}

TEST(UnstructuredMesh, get_chart)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    Int start, end;
    m->get_chart(start, end);
    EXPECT_EQ(start, 0);
    EXPECT_EQ(end, 45);

    auto range = m->get_chart();
    EXPECT_EQ(range.first(), 0);
    EXPECT_EQ(range.last(), 45);
}

TEST(UnstructuredMesh, common_cells_by_vertex)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    auto map = m->common_cells_by_vertex();
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
    auto map1 = m->common_cells_by_vertex();
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
    class TestMesh2D : public MeshObject {
    public:
        explicit TestMesh2D(const godzilla::Parameters & parameters) : MeshObject(parameters) {}

        Mesh *
        create_mesh() override
        {
            std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
            std::vector<Real> vertices = { 0, 0, 1, 0, 0, 1, 1, 1 };
            return UnstructuredMesh::build_from_cell_list(get_comm(),
                                                          2,
                                                          3,
                                                          cells,
                                                          2,
                                                          vertices,
                                                          true);
        }
    };

    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    EXPECT_EQ(m->get_num_cells(), 2);
    EXPECT_THAT(m->get_connectivity(0), ElementsAre(2, 3, 4));
    EXPECT_THAT(m->get_connectivity(1), ElementsAre(3, 5, 4));
    EXPECT_EQ(m->get_num_vertices(), 4);
    auto coords = m->get_coordinates();
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

TEST(UnstructuredMesh, mark_boundary_faces)
{
    class TestMesh2D : public MeshObject {
    public:
        explicit TestMesh2D(const godzilla::Parameters & parameters) : MeshObject(parameters) {}

        Mesh *
        create_mesh() override
        {
            std::vector<Int> cells = { 0, 1, 2, 1, 3, 2 };
            std::vector<Real> vertices = { 0, 0, 1, 0, 0, 1, 1, 1 };
            auto m =
                UnstructuredMesh::build_from_cell_list(get_comm(), 2, 3, cells, 2, vertices, true);
            auto face_sets = m->create_label("face sets");
            m->mark_boundary_faces(10, face_sets);
            return m;
        }
    };

    TestApp app;

    Parameters mesh_pars = TestMesh2D::parameters();
    mesh_pars.set<godzilla::App *>("_app") = &app;
    TestMesh2D mesh(mesh_pars);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    auto face_set = m->get_label("face sets");
    auto facets = face_set.get_stratum(10);
    facets.get_indices();
    EXPECT_EQ(facets.get_local_size(), 4);
    auto facets_idxs = facets.to_std_vector();
    EXPECT_THAT(facets_idxs, ElementsAre(6, 8, 9, 10));
    facets.restore_indices();
}

TEST(UnstructuredMesh, point_star_forrest)
{
    TestApp app;
    auto comm = app.get_comm();

    Parameters params = TestUnstructuredMesh::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    TestUnstructuredMesh mesh(params);
    mesh.create();
    auto m = mesh.get_mesh<UnstructuredMesh>();

    StarForest sf;
    sf.create(comm);

    m->set_point_star_forest(sf);
    EXPECT_EQ(static_cast<PetscSF>(m->get_point_star_forest()), static_cast<PetscSF>(sf));

    sf.destroy();
}

TEST(UnstructuredMesh, face_sets)
{
    TestApp app;

    Parameters params = TestUnstructuredMesh3D::parameters();
    params.set<App *>("_app") = &app;
    params.set<std::string>("_name") = "obj";
    params.set<Int>("nx") = 2;
    params.set<Int>("ny") = 1;
    params.set<Int>("nz") = 1;
    TestUnstructuredMesh3D mesh(params);
    mesh.create();

    auto m = mesh.get_mesh<UnstructuredMesh>();
    EXPECT_EQ(m->get_num_face_sets(), 6);
    EXPECT_THAT(m->get_face_sets(),
                ElementsAre(Pair(1, "back"),
                            Pair(2, "front"),
                            Pair(3, "bottom"),
                            Pair(4, "top"),
                            Pair(5, "right"),
                            Pair(6, "left")));

    EXPECT_EQ(m->get_face_set_name(1), "back");
    EXPECT_EQ(m->get_face_set_name(2), "front");
    EXPECT_EQ(m->get_face_set_name(3), "bottom");
    EXPECT_EQ(m->get_face_set_name(4), "top");
    EXPECT_EQ(m->get_face_set_name(5), "right");
    EXPECT_EQ(m->get_face_set_name(6), "left");

    EXPECT_TRUE(m->has_face_set("back"));
    EXPECT_TRUE(m->has_face_set("front"));
    EXPECT_TRUE(m->has_face_set("bottom"));
    EXPECT_TRUE(m->has_face_set("top"));
    EXPECT_TRUE(m->has_face_set("right"));
    EXPECT_TRUE(m->has_face_set("left"));
}
