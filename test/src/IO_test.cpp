#include "gmock/gmock.h"
#include "TestApp.h"
#include "GTest2FieldsFENonlinearProblem.h"
#include "GTestDGLinearProblem.h"
#include "godzilla/IO.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshFactory.h"
#include "godzilla/LineMesh.h"
#include "godzilla/RectangleMesh.h"
#include "godzilla/BoxMesh.h"
#include "godzilla/Types.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include <filesystem>

using namespace godzilla;
namespace fs = std::filesystem;

// Helpers tests

TEST(IOTest, get_elem_type_segment)
{
    auto type = io::get_elem_type(PolytopeType::SEGMENT);
    EXPECT_STREQ(type, "BAR2");
}

TEST(IOTest, get_elem_type_triangle)
{
    auto type = io::get_elem_type(PolytopeType::TRIANGLE);
    EXPECT_STREQ(type, "TRI3");
}

TEST(IOTest, get_elem_type_quad)
{
    auto type = io::get_elem_type(PolytopeType::QUADRILATERAL);
    EXPECT_STREQ(type, "QUAD4");
}

TEST(IOTest, get_elem_type_tet)
{
    auto type = io::get_elem_type(PolytopeType::TETRAHEDRON);
    EXPECT_STREQ(type, "TET4");
}

TEST(IOTest, get_elem_type_hex)
{
    auto type = io::get_elem_type(PolytopeType::HEXAHEDRON);
    EXPECT_STREQ(type, "HEX8");
}

TEST(IOTest, get_elem_node_ordering_segment)
{
    auto ordering = io::get_elem_node_ordering(PolytopeType::SEGMENT);
    EXPECT_EQ(ordering[0], 0);
    EXPECT_EQ(ordering[1], 1);
}

TEST(IOTest, get_elem_node_ordering_triangle)
{
    auto ordering = io::get_elem_node_ordering(PolytopeType::TRIANGLE);
    EXPECT_EQ(ordering[0], 0);
    EXPECT_EQ(ordering[1], 1);
    EXPECT_EQ(ordering[2], 2);
}

TEST(IOTest, get_elem_side_ordering_segment)
{
    auto ordering = io::get_elem_side_ordering(PolytopeType::SEGMENT);
    EXPECT_EQ(ordering[0], 1);
    EXPECT_EQ(ordering[1], 2);
}

TEST(IOTest, get_elem_side_ordering_triangle)
{
    auto ordering = io::get_elem_side_ordering(PolytopeType::TRIANGLE);
    EXPECT_EQ(ordering[0], 1);
    EXPECT_EQ(ordering[1], 2);
    EXPECT_EQ(ordering[2], 3);
}

// 1-D CG mesh write tests

TEST(IOTest, write_mesh_1d_cg)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 3);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    fs::path file_path = "test_mesh_1d.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    io::write_mesh(exo, *mesh);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // 1D mesh with nx=3 should have nx+1 = 4 nodes and nx = 3 elements
    EXPECT_EQ(exo_read.get_dim(), 1);
    EXPECT_EQ(exo_read.get_num_nodes(), 4);
    EXPECT_EQ(exo_read.get_num_elements(), 3);
    EXPECT_EQ(exo_read.get_num_element_blocks(), 1);

    // Verify coordinates
    auto x_coords = exo_read.get_x_coords();
    EXPECT_EQ(x_coords.size(), 4);
    EXPECT_NEAR(x_coords[0], 0.0, 1e-15);
    EXPECT_NEAR(x_coords[1], 1.0 / 3.0, 1e-15);
    EXPECT_NEAR(x_coords[2], 2.0 / 3.0, 1e-15);
    EXPECT_NEAR(x_coords[3], 1.0, 1e-15);

    // Verify element block
    auto blocks = exo_read.get_element_blocks();
    ASSERT_EQ(blocks.size(), 1);
    EXPECT_STREQ(blocks[0].get_element_type().c_str(), "BAR2");
    EXPECT_EQ(blocks[0].get_size(), 3);
    EXPECT_EQ(blocks[0].get_num_nodes_per_element(), 2);

    // Verify connectivity
    auto connectivity = blocks[0].get_connectivity();
    EXPECT_EQ(connectivity.size(), 6); // 3 elements * 2 nodes per element
    // Elements should be 1-2, 2-3, 3-4 (1-based indexing)
    EXPECT_EQ(connectivity[0], 1);
    EXPECT_EQ(connectivity[1], 2);
    EXPECT_EQ(connectivity[2], 2);
    EXPECT_EQ(connectivity[3], 3);
    EXPECT_EQ(connectivity[4], 3);
    EXPECT_EQ(connectivity[5], 4);

    exo_read.close();

    fs::remove(file_path);
}

// 2-D CG mesh write tests

TEST(IOTest, write_mesh_2d_cg)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<RectangleMesh>();
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 2);
    auto mesh = MeshFactory::create<RectangleMesh>(mesh_pars);

    fs::path file_path = "test_mesh_2d.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    io::write_mesh(exo, *mesh);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // 2D mesh with nx=2, ny=2 should have (nx+1)*(ny+1) = 9 nodes and nx*ny = 4 elements
    EXPECT_EQ(exo_read.get_dim(), 2);
    EXPECT_EQ(exo_read.get_num_nodes(), 9);
    EXPECT_EQ(exo_read.get_num_elements(), 4);
    EXPECT_EQ(exo_read.get_num_element_blocks(), 1);

    // Verify coordinates
    // 2x2 mesh has 3x3 grid of nodes with coordinates [0, 0.5, 1] in each direction
    auto x_coords = exo_read.get_x_coords();
    auto y_coords = exo_read.get_y_coords();
    EXPECT_EQ(x_coords.size(), 9);
    EXPECT_EQ(y_coords.size(), 9);

    // Check corner nodes
    EXPECT_NEAR(x_coords[0], 0.0, 1e-15); // node 0: (0, 0)
    EXPECT_NEAR(y_coords[0], 0.0, 1e-15);
    EXPECT_NEAR(x_coords[2], 1.0, 1e-15); // node 2: (1, 0)
    EXPECT_NEAR(y_coords[2], 0.0, 1e-15);
    EXPECT_NEAR(x_coords[6], 0.0, 1e-15); // node 6: (0, 1)
    EXPECT_NEAR(y_coords[6], 1.0, 1e-15);
    EXPECT_NEAR(x_coords[8], 1.0, 1e-15); // node 8: (1, 1)
    EXPECT_NEAR(y_coords[8], 1.0, 1e-15);

    // Check center node
    EXPECT_NEAR(x_coords[4], 0.5, 1e-15); // node 4: (0.5, 0.5)
    EXPECT_NEAR(y_coords[4], 0.5, 1e-15);

    // Verify element block
    auto blocks = exo_read.get_element_blocks();
    ASSERT_EQ(blocks.size(), 1);
    EXPECT_STREQ(blocks[0].get_element_type().c_str(), "QUAD4");
    EXPECT_EQ(blocks[0].get_size(), 4);
    EXPECT_EQ(blocks[0].get_num_nodes_per_element(), 4);

    // Verify connectivity
    // For a 2x2 mesh, nodes are numbered 0-8 (left to right, bottom to top):
    // 6---7---8
    // |   |   |
    // 3---4---5
    // |   |   |
    // 0---1---2
    // Quads counter-clockwise (1-based): (0,1,4,3), (1,2,5,4), (3,4,7,6), (4,5,8,7)
    auto connectivity = blocks[0].get_connectivity();
    EXPECT_EQ(connectivity.size(), 16); // 4 elements * 4 nodes per element

    // Quad 0 (bottom-left): 1, 2, 5, 4
    EXPECT_EQ(connectivity[0], 1);
    EXPECT_EQ(connectivity[1], 2);
    EXPECT_EQ(connectivity[2], 5);
    EXPECT_EQ(connectivity[3], 4);

    // Quad 1 (bottom-right): 2, 3, 6, 5
    EXPECT_EQ(connectivity[4], 2);
    EXPECT_EQ(connectivity[5], 3);
    EXPECT_EQ(connectivity[6], 6);
    EXPECT_EQ(connectivity[7], 5);

    // Quad 2 (top-left): 4, 5, 8, 7
    EXPECT_EQ(connectivity[8], 4);
    EXPECT_EQ(connectivity[9], 5);
    EXPECT_EQ(connectivity[10], 8);
    EXPECT_EQ(connectivity[11], 7);

    // Quad 3 (top-right): 5, 6, 9, 8
    EXPECT_EQ(connectivity[12], 5);
    EXPECT_EQ(connectivity[13], 6);
    EXPECT_EQ(connectivity[14], 9);
    EXPECT_EQ(connectivity[15], 8);

    exo_read.close();

    fs::remove(file_path);
}

// 3-D CG mesh write tests

TEST(IOTest, write_mesh_3d_cg)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<BoxMesh>();
    mesh_pars.set<Int>("nx", 2);
    mesh_pars.set<Int>("ny", 2);
    mesh_pars.set<Int>("nz", 2);
    auto mesh = MeshFactory::create<BoxMesh>(mesh_pars);

    fs::path file_path = "test_mesh_3d.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    io::write_mesh(exo, *mesh);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // 3D mesh with nx=2, ny=2, nz=2 should have (nx+1)*(ny+1)*(nz+1) = 27 nodes and nx*ny*nz = 8
    // elements
    EXPECT_EQ(exo_read.get_dim(), 3);
    EXPECT_EQ(exo_read.get_num_nodes(), 27);
    EXPECT_EQ(exo_read.get_num_elements(), 8);
    EXPECT_EQ(exo_read.get_num_element_blocks(), 1);

    // Verify coordinates
    // 2x2x2 mesh has 3x3x3 grid of nodes with coordinates [0, 0.5, 1] in each direction
    auto x_coords = exo_read.get_x_coords();
    auto y_coords = exo_read.get_y_coords();
    auto z_coords = exo_read.get_z_coords();
    EXPECT_EQ(x_coords.size(), 27);
    EXPECT_EQ(y_coords.size(), 27);
    EXPECT_EQ(z_coords.size(), 27);

    // Check corner nodes
    // Node 0: (0, 0, 0)
    EXPECT_NEAR(x_coords[0], 0.0, 1e-15);
    EXPECT_NEAR(y_coords[0], 0.0, 1e-15);
    EXPECT_NEAR(z_coords[0], 0.0, 1e-15);

    // Node 2: (1, 0, 0)
    EXPECT_NEAR(x_coords[2], 1.0, 1e-15);
    EXPECT_NEAR(y_coords[2], 0.0, 1e-15);
    EXPECT_NEAR(z_coords[2], 0.0, 1e-15);

    // Node 26: (1, 1, 1)
    EXPECT_NEAR(x_coords[26], 1.0, 1e-15);
    EXPECT_NEAR(y_coords[26], 1.0, 1e-15);
    EXPECT_NEAR(z_coords[26], 1.0, 1e-15);

    // Node 13: (0.5, 0.5, 0.5) - center node
    EXPECT_NEAR(x_coords[13], 0.5, 1e-15);
    EXPECT_NEAR(y_coords[13], 0.5, 1e-15);
    EXPECT_NEAR(z_coords[13], 0.5, 1e-15);

    // Verify element block
    auto blocks = exo_read.get_element_blocks();
    ASSERT_EQ(blocks.size(), 1);
    EXPECT_STREQ(blocks[0].get_element_type().c_str(), "HEX8");
    EXPECT_EQ(blocks[0].get_size(), 8);
    EXPECT_EQ(blocks[0].get_num_nodes_per_element(), 8);

    // Verify connectivity
    // For a 2x2x2 mesh, nodes are numbered 0-26 (left to right, bottom to top, bottom to top
    // layers):
    // Bottom layer (z=0):   0-8   (3x3 grid)
    // Middle layer (z=1):   9-17  (3x3 grid)
    // Top layer (z=2):      18-26 (3x3 grid)
    //
    // Hex 0 (bottom-left-front): (0,1,4,3,9,10,13,12) -> 1-based: (1,2,5,4,10,11,14,13)
    // Hex 1 (bottom-right-front): (1,2,5,4,10,11,14,13) -> 1-based: (2,3,6,5,11,12,15,14)
    auto connectivity = blocks[0].get_connectivity();
    EXPECT_EQ(connectivity.size(), 64); // 8 elements * 8 nodes per element

    // Hex 0 (bottom-left-front)
    EXPECT_EQ(connectivity[0], 1);
    EXPECT_EQ(connectivity[1], 2);
    EXPECT_EQ(connectivity[2], 5);
    EXPECT_EQ(connectivity[3], 4);
    EXPECT_EQ(connectivity[4], 10);
    EXPECT_EQ(connectivity[5], 11);
    EXPECT_EQ(connectivity[6], 14);
    EXPECT_EQ(connectivity[7], 13);

    // Hex 1 (bottom-right-front)
    EXPECT_EQ(connectivity[8], 2);
    EXPECT_EQ(connectivity[9], 3);
    EXPECT_EQ(connectivity[10], 6);
    EXPECT_EQ(connectivity[11], 5);
    EXPECT_EQ(connectivity[12], 11);
    EXPECT_EQ(connectivity[13], 12);
    EXPECT_EQ(connectivity[14], 15);
    EXPECT_EQ(connectivity[15], 14);

    exo_read.close();

    fs::remove(file_path);
}

// DG mesh write tests

TEST(IOTest, write_mesh_discontinuous)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    fs::path file_path = "test_mesh_dg.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    io::write_mesh_discontinuous(exo, *mesh);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // For discontinuous (DG) mesh, nodes are duplicated per element
    // nx=2 means 2 elements, each with 2 nodes, so 4 total nodes
    EXPECT_EQ(exo_read.get_dim(), 1);
    EXPECT_EQ(exo_read.get_num_nodes(), 4);
    EXPECT_EQ(exo_read.get_num_elements(), 2);
    EXPECT_EQ(exo_read.get_num_element_blocks(), 1);

    // Verify coordinates
    // DG mesh has coordinates [0, 0.5, 0.5, 1] for 4 DG nodes
    auto x_coords_dg = exo_read.get_x_coords();
    EXPECT_EQ(x_coords_dg.size(), 4);
    EXPECT_NEAR(x_coords_dg[0], 0.0, 1e-15); // Element 0, local node 1
    EXPECT_NEAR(x_coords_dg[1], 0.5, 1e-15); // Element 0, local node 2
    EXPECT_NEAR(x_coords_dg[2], 0.5, 1e-15); // Element 1, local node 1
    EXPECT_NEAR(x_coords_dg[3], 1.0, 1e-15); // Element 1, local node 2

    // Verify element block
    auto blocks = exo_read.get_element_blocks();
    ASSERT_EQ(blocks.size(), 1);
    EXPECT_STREQ(blocks[0].get_element_type().c_str(), "BAR2");
    EXPECT_EQ(blocks[0].get_size(), 2);
    EXPECT_EQ(blocks[0].get_num_nodes_per_element(), 2);

    // Verify connectivity (each element uses its own local nodes 1-2, 3-4)
    auto connectivity = blocks[0].get_connectivity();
    EXPECT_EQ(connectivity.size(), 4); // 2 elements * 2 nodes per element
    EXPECT_EQ(connectivity[0], 1);
    EXPECT_EQ(connectivity[1], 2);
    EXPECT_EQ(connectivity[2], 3);
    EXPECT_EQ(connectivity[3], 4);

    exo_read.close();

    fs::remove(file_path);
}

// Variable naming tests

TEST(IOTest, get_var_names)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<GTest2FieldsFENonlinearProblem>();
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTest2FieldsFENonlinearProblem prob(prob_pars);

    auto aux_fld1_fid = prob.add_aux_field("aux_fld1", 2, Order(1));
    prob.set_aux_field_component_name(aux_fld1_fid, 0, "aux_fld1_x");
    prob.set_aux_field_component_name(aux_fld1_fid, 1, "aux_fld1_y");

    prob.create();

    // Test "u" field
    auto u_field_id = prob.get_field_id("u").value();
    auto u_var_names = io::get_var_names(prob, u_field_id);

    // The "u" field is a scalar (1 component), so we should get just "u"
    EXPECT_EQ(u_var_names.size(), 1);
    EXPECT_STREQ(u_var_names[0].c_str(), "u");

    // Test "v" field
    auto v_field_id = prob.get_field_id("v").value();
    auto v_var_names = io::get_var_names(prob, v_field_id);

    // The "v" field is a scalar (1 component), so we should get just "v"
    EXPECT_EQ(v_var_names.size(), 1);
    EXPECT_STREQ(v_var_names[0].c_str(), "v");

    // Test auxiliary field names
    auto aux_var_names = io::get_aux_var_names(prob, aux_fld1_fid);

    // The auxiliary field has 2 components with custom names
    EXPECT_EQ(aux_var_names.size(), 2);
    EXPECT_STREQ(aux_var_names[0].c_str(), "aux_fld1_x");
    EXPECT_STREQ(aux_var_names[1].c_str(), "aux_fld1_y");
}

// Field value writing tests

TEST(IOTest, write_field_values)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<GTest2FieldsFENonlinearProblem>();
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh));
    GTest2FieldsFENonlinearProblem prob(prob_pars);

    FieldID elem1_fid(2);
    prob.set_field(elem1_fid, "elem1", 1, Order(0));

    auto aux_fld1_fid = prob.add_aux_field("aux_fld1", 2, Order(1));
    prob.set_aux_field_component_name(aux_fld1_fid, 0, "aux_fld1_x");
    prob.set_aux_field_component_name(aux_fld1_fid, 1, "aux_fld1_y");

    auto ae1_fid = prob.add_aux_field("ae1", 2, Order(0));
    prob.set_aux_field_component_name(ae1_fid, 0, "ae1_x");
    prob.set_aux_field_component_name(ae1_fid, 1, "ae1_y");

    prob.create();

    // Get field IDs
    auto u_fid = prob.get_field_id("u").value();
    auto v_fid = prob.get_field_id("v").value();

    // Set some solution values on the global solution vector
    auto sol = prob.get_solution_vector();
    auto sol_array = sol.borrow_array();
    for (Int i = 0; i < sol.get_size(); i++)
        sol_array[i] = 1.0 + i * 0.5;

    auto aux_sln = prob.get_aux_solution_vector_local();
    auto aux_sln_vals = aux_sln.borrow_array();
    for (Int i = 0; i < aux_sln.get_size(); i++)
        aux_sln_vals[i] = 10.0 - i * 0.5;

    // Compute local solution vector from global
    prob.compute_solution_vector_local();

    // Create ExodusII file
    fs::path file_path = "test_write_field_values.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    // Write mesh (this also initializes the file)
    io::write_mesh(exo, *mesh);

    // Write nodal variable names for "u" field
    exo.write_nodal_var_names({ "u", "v", "aux_x", "aux_y" });
    exo.write_elem_var_names({ "elem1", "ae1_x", "ae1_y" });

    // Write time step
    int step_num = 1;
    double time_val = 0.0;
    exo.write_time(step_num, time_val);

    // Write field values for "u" field
    io::write_field_values(exo, prob, step_num, time_val, u_fid, 1);
    io::write_field_values(exo, prob, step_num, time_val, v_fid, 2);
    io::write_aux_field_values(exo, prob, step_num, time_val, aux_fld1_fid, 3);

    io::write_elemental_field_values(exo, prob, step_num, time_val, elem1_fid, 1);
    io::write_aux_elemental_field_values(exo, prob, step_num, time_val, ae1_fid, 2);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // Verify time step
    EXPECT_EQ(exo_read.get_num_times(), 1);
    auto times = exo_read.get_times();
    EXPECT_NEAR(times[0], 0.0, 1e-15);

    auto u_values = exo_read.get_nodal_variable_values(step_num, 1);
    EXPECT_EQ(u_values.size(), 3);
    EXPECT_NEAR(u_values[0], 2.0, 1e-15);
    EXPECT_NEAR(u_values[1], 3.0, 1e-15);
    EXPECT_NEAR(u_values[2], 4.0, 1e-15);

    auto v_values = exo_read.get_nodal_variable_values(step_num, 2);
    EXPECT_EQ(v_values.size(), 3);
    EXPECT_NEAR(v_values[0], 2.5, 1e-15);
    EXPECT_NEAR(v_values[1], 3.5, 1e-15);
    EXPECT_NEAR(v_values[2], 4.5, 1e-15);

    auto aux_fld1_x_values = exo_read.get_nodal_variable_values(step_num, 3);
    EXPECT_EQ(aux_fld1_x_values.size(), 3);
    EXPECT_NEAR(aux_fld1_x_values[0], 8., 1e-15);
    EXPECT_NEAR(aux_fld1_x_values[1], 7., 1e-15);
    EXPECT_NEAR(aux_fld1_x_values[2], 6., 1e-15);

    auto aux_fld1_y_values = exo_read.get_nodal_variable_values(step_num, 4);
    EXPECT_EQ(aux_fld1_y_values.size(), 3);
    EXPECT_NEAR(aux_fld1_y_values[0], 7.5, 1e-15);
    EXPECT_NEAR(aux_fld1_y_values[1], 6.5, 1e-15);
    EXPECT_NEAR(aux_fld1_y_values[2], 5.5, 1e-15);

    auto elem1_values = exo_read.get_elemental_variable_values(step_num, 1, 0);
    EXPECT_EQ(elem1_values.size(), 2);
    EXPECT_NEAR(elem1_values[0], 1, 1e-15);
    EXPECT_NEAR(elem1_values[1], 1.5, 1e-15);

    auto ae1_x_values = exo_read.get_elemental_variable_values(step_num, 2, 0);
    EXPECT_EQ(ae1_x_values.size(), 2);
    EXPECT_NEAR(ae1_x_values[0], 10, 1e-15);
    EXPECT_NEAR(ae1_x_values[1], 9., 1e-15);

    auto ae1_y_values = exo_read.get_elemental_variable_values(step_num, 3, 0);
    EXPECT_EQ(ae1_y_values.size(), 2);
    EXPECT_NEAR(ae1_y_values[0], 9.5, 1e-15);
    EXPECT_NEAR(ae1_y_values[1], 8.5, 1e-15);

    exo_read.close();

    fs::remove(file_path);
}

// DG field value writing test

TEST(IOTest, write_field_values_dg_simple)
{
    TestApp app;

    auto mesh_pars = app.make_parameters<LineMesh>();
    mesh_pars.set<Int>("nx", 2);
    auto mesh = MeshFactory::create<LineMesh>(mesh_pars);

    auto prob_pars = app.make_parameters<GTestDGLinearProblem>();
    prob_pars.set<Ref<Mesh>>("mesh", ref(*mesh)).set<Real>("dt", 1.).set<Int>("num_steps", 1);
    GTestDGLinearProblem prob(prob_pars);

    auto aux_fld1_fid = prob.add_aux_field("aux_fld1", 2, Order(0));
    prob.set_aux_field_component_name(aux_fld1_fid, 0, "aux_fld1_x");
    prob.set_aux_field_component_name(aux_fld1_fid, 1, "aux_fld1_y");

    prob.create();

    // Get field ID
    auto u_fid = prob.get_field_id("u").value();

    // Set some solution values on the global solution vector
    auto sol = prob.get_solution_vector_local();
    auto sol_array = sol.borrow_array();
    for (Int i = 0; i < sol.get_size(); i++)
        sol_array[i] = 1.0 + i * 0.5;

    auto aux_sln = prob.get_aux_solution_vector_local();
    auto aux_sln_vals = aux_sln.borrow_array();
    for (Int i = 0; i < aux_sln.get_size(); i++)
        aux_sln_vals[i] = 10.0 - i * 0.5;

    // Create ExodusII file
    fs::path file_path = "test_write_field_values_dg.exo";
    exodusIIcpp::File exo(file_path, exodusIIcpp::FileAccess::WRITE);
    ASSERT_TRUE(exo.is_opened());

    // Write mesh (this also initializes the file)
    io::write_mesh_discontinuous(exo, *mesh);

    // Write nodal variable names
    exo.write_nodal_var_names({ "u", "aux_fld1_x", "aux_fld1_y" });

    // Write time step
    int step_num = 1;
    double time_val = 0.0;
    exo.write_time(step_num, time_val);

    // Write field values
    io::write_field_values(exo, prob, step_num, time_val, u_fid, 1);
    io::write_aux_field_values(exo, prob, step_num, time_val, aux_fld1_fid, 2);

    exo.close();

    // Verify the file content
    exodusIIcpp::File exo_read(file_path, exodusIIcpp::FileAccess::READ);
    ASSERT_TRUE(exo_read.is_opened());
    exo_read.read();

    // Verify time step
    EXPECT_EQ(exo_read.get_num_times(), 1);
    auto times = exo_read.get_times();
    EXPECT_NEAR(times[0], 0.0, 1e-15);

    // Verify "u" field nodal values (var_id=1)
    auto u_values = exo_read.get_nodal_variable_values(step_num, 1);
    EXPECT_EQ(u_values.size(), 4);
    EXPECT_NEAR(u_values[0], 1., 1e-15);
    EXPECT_NEAR(u_values[1], 1.5, 1e-15);
    EXPECT_NEAR(u_values[2], 2., 1e-15);
    EXPECT_NEAR(u_values[3], 2.5, 1e-15);

    // Verify auxiliary field nodal values (var_id=2,3)
    auto aux_fld1_x_values = exo_read.get_nodal_variable_values(step_num, 2);
    EXPECT_EQ(aux_fld1_x_values.size(), 4);
    EXPECT_NEAR(aux_fld1_x_values[0], 10., 1e-15);
    EXPECT_NEAR(aux_fld1_x_values[1], 9., 1e-15);
    EXPECT_NEAR(aux_fld1_x_values[2], 8., 1e-15);
    EXPECT_NEAR(aux_fld1_x_values[3], 7., 1e-15);

    auto aux_fld1_y_values = exo_read.get_nodal_variable_values(step_num, 3);
    EXPECT_EQ(aux_fld1_y_values.size(), 4);
    EXPECT_NEAR(aux_fld1_y_values[0], 9.5, 1e-15);
    EXPECT_NEAR(aux_fld1_y_values[1], 8.5, 1e-15);
    EXPECT_NEAR(aux_fld1_y_values[2], 7.5, 1e-15);
    EXPECT_NEAR(aux_fld1_y_values[3], 6.5, 1e-15);

    exo_read.close();

    fs::remove(file_path);
}
