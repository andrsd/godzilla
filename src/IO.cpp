// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/IO.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/DGProblemInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "petscdmplex.h"
#include <set>

namespace godzilla {
namespace io {

namespace {

/// Helper: get number of nodes per element for a given polytope type
int
num_nodes_per_element(const UnstructuredMesh & mesh)
{
    int n_nodes_per_elem = 0;
    int n_elems = (int) mesh.get_num_cells();
    if (n_elems > 0) {
        auto ct = mesh.get_cell_type(0);
        n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(ct);
    }
    return n_nodes_per_elem;
}

struct ContinousPolicy {
    static auto
    write_block(exodusIIcpp::File & f,
                const UnstructuredMesh & mesh,
                int blk_id,
                PolytopeType polytope_type,
                Int n_elems_in_block,
                const Int * cells)
    {
        CALL_STACK_MSG();
        auto dm = mesh.get_dm();
        Int n_all_elems = mesh.get_num_all_cells();
        auto elem_type = get_elem_type(polytope_type);
        int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);
        auto ordering = get_elem_node_ordering(polytope_type);
        std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
        for (Int i = 0, j = 0; i < n_elems_in_block; ++i) {
            Int elem_id = cells[i];
            Int closure_size;
            Int * closure = nullptr;
            PETSC_CHECK(
                DMPlexGetTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
            for (Int k = 0; k < n_nodes_per_elem; ++k, ++j) {
                Int l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
                connect[j] = (int) (closure[l] - n_all_elems + 1);
            }
            PETSC_CHECK(
                DMPlexRestoreTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
        }
        f.write_block(blk_id, elem_type, n_elems_in_block, connect);
    }
};

struct DiscontinousPolicy {
    static auto
    write_block(exodusIIcpp::File & f,
                const UnstructuredMesh & mesh,
                int blk_id,
                PolytopeType polytope_type,
                Int n_elems_in_block,
                const Int * cells)
    {
        CALL_STACK_MSG();
        auto elem_type = get_elem_type(polytope_type);
        int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);
        std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
        for (Int i = 0, j = 0; i < n_elems_in_block; ++i) {
            auto cell_id = cells[i];
            for (Int k = 0; k < n_nodes_per_elem; ++k, ++j)
                connect[j] = (int) (cell_id * n_nodes_per_elem + k + 1);
        }
        f.write_block(blk_id, elem_type, n_elems_in_block, connect);
    }
};

struct PrimaryFieldPolicy {
    static auto
    get_solution(const DiscreteProblemInterface & dpi)
    {
        return dpi.get_solution_vector_local();
    }

    static auto
    get_name(const DiscreteProblemInterface & dpi, FieldID fid)
    {
        return dpi.get_field_name(fid);
    }

    static auto
    get_num_components(const DiscreteProblemInterface & dpi, FieldID fid)
    {
        return dpi.get_field_num_components(fid);
    }

    static auto
    get_component_name(const DiscreteProblemInterface & dpi, FieldID fid, Int component)
    {
        return dpi.get_field_component_name(fid, component);
    }

    static auto
    get_dof(const DiscreteProblemInterface & dpi, Int n, FieldID fid)
    {
        return dpi.get_field_dof(n, fid);
    }

    static auto
    get_dof(const DGProblemInterface & dgpi, Int cell, Int n, FieldID fid)
    {
        return dgpi.get_field_dof(cell, n, fid);
    }
};

struct AuxFieldPolicy {
    static auto
    get_solution(const DiscreteProblemInterface & dpi)
    {
        return dpi.get_aux_solution_vector_local();
    }

    static auto
    get_name(const DiscreteProblemInterface & dpi, FieldID fid)
    {
        return dpi.get_aux_field_name(fid);
    }

    static auto
    get_num_components(const DiscreteProblemInterface & dpi, FieldID fid)
    {
        return dpi.get_aux_field_num_components(fid);
    }

    static auto
    get_component_name(const DiscreteProblemInterface & dpi, FieldID fid, Int component)
    {
        return dpi.get_aux_field_component_name(fid, component);
    }

    static auto
    get_dof(const DiscreteProblemInterface & dpi, Int n, FieldID fid)
    {
        return dpi.get_aux_field_dof(n, fid);
    }

    static auto
    get_dof(const DGProblemInterface & dgpi, Int cell, Int n, FieldID fid)
    {
        return dgpi.get_aux_field_dof(cell, n, fid);
    }
};

} // anonymous namespace

// Helpers

const char *
get_elem_type(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    switch (elem_type) {
    case PolytopeType::SEGMENT:
        return "BAR2";
    case PolytopeType::TRIANGLE:
        return "TRI3";
    case PolytopeType::QUADRILATERAL:
        return "QUAD4";
    case PolytopeType::TETRAHEDRON:
        return "TET4";
    case PolytopeType::HEXAHEDRON:
        return "HEX8";
    default:
        throw InternalError("Unsupported type.");
    }
}

Span<const Int>
get_elem_node_ordering(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    static const Int seg_ordering[] = { 0, 1 };
    static const Int tri_ordering[] = { 0, 1, 2 };
    static const Int quad_ordering[] = { 0, 1, 2, 3 };
    static const Int tet_ordering[] = { 1, 0, 2, 3 };
    static const Int hex_ordering[] = { 0, 3, 2, 1, 4, 5, 6, 7, 8 };

    switch (elem_type) {
    case PolytopeType::SEGMENT:
        return seg_ordering;
    case PolytopeType::TRIANGLE:
        return tri_ordering;
    case PolytopeType::QUADRILATERAL:
        return quad_ordering;
    case PolytopeType::TETRAHEDRON:
        return tet_ordering;
    case PolytopeType::HEXAHEDRON:
        return hex_ordering;
    default:
        throw InternalError("Unsupported type.");
    }
}

Span<const Int>
get_elem_side_ordering(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    static const Int seg_ordering[] = { 1, 2 };
    static const Int tri_ordering[] = { 1, 2, 3 };
    static const Int quad_ordering[] = { 1, 2, 3, 4 };
    static const Int tet_ordering[] = { 4, 1, 2, 3 };
    static const Int hex_ordering[] = { 5, 6, 1, 3, 2, 4 };

    switch (elem_type) {
    case PolytopeType::SEGMENT:
        return seg_ordering;
    case PolytopeType::TRIANGLE:
        return tri_ordering;
    case PolytopeType::QUADRILATERAL:
        return quad_ordering;
    case PolytopeType::TETRAHEDRON:
        return tet_ordering;
    case PolytopeType::HEXAHEDRON:
        return hex_ordering;
    default:
        throw InternalError("Unsupported type.");
    }
}

// Low-level mesh writing primitives

template <typename Policy>
void write_mesh_init(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

template <>
void
write_mesh_init<ContinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    int n_nodes = (int) mesh.get_num_vertices();
    int n_elems = (int) mesh.get_num_cells();

    // number of element blocks
    int n_elem_blk = (int) mesh.get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int n_node_sets = (int) mesh.get_num_vertex_sets();
    int n_side_sets = (int) mesh.get_num_face_sets();

    int exo_dim = mesh.get_dimension();
    f.init("", exo_dim, n_nodes, n_elems, n_elem_blk, n_node_sets, n_side_sets);
}

template <>
void
write_mesh_init<DiscontinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    int n_elems = (int) mesh.get_num_cells();
    int n_nodes_per_elem = num_nodes_per_element(mesh);
    int n_nodes = (int) n_elems * n_nodes_per_elem;

    // number of element blocks
    int n_elem_blk = (int) mesh.get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int exo_dim = mesh.get_dimension();
    f.init("", exo_dim, n_nodes, n_elems, n_elem_blk, 0, 0);
}

template <typename Policy>
void write_coords(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

template <>
void
write_coords<ContinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    int exo_dim = mesh.get_dimension();
    int dim = mesh.get_dimension();
    auto coord = mesh.get_coordinates_local();
    auto coord_size = coord.get_size();
    auto xyz = coord.get_array();

    int n_nodes = (int) coord_size / dim;
    std::vector<double> x(n_nodes, 0.);
    std::vector<double> y;
    std::vector<double> z;

    if (exo_dim >= 2)
        y.resize(n_nodes);
    if (exo_dim >= 3)
        z.resize(n_nodes);
    for (Int i = 0; i < n_nodes; ++i) {
        x[i] = xyz[i * dim + 0];
        if (dim >= 2)
            y[i] = xyz[i * dim + 1];
        if (dim >= 3)
            z[i] = xyz[i * dim + 2];
    }

    if (exo_dim == 1)
        f.write_coords(x);
    else if (exo_dim == 2)
        f.write_coords(x, y);
    else if (exo_dim == 3)
        f.write_coords(x, y, z);

    coord.restore_array(xyz);

    f.write_coord_names();
}

template <>
void
write_coords<DiscontinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    int exo_dim = mesh.get_dimension();
    int dim = mesh.get_dimension();
    auto coord = mesh.get_coordinates_local();
    auto xyz = coord.get_array();

    int n_elems = mesh.get_num_cells();
    int n_nodes_per_elem = num_nodes_per_element(mesh);
    int n_nodes = n_elems * n_nodes_per_elem;

    std::vector<double> x(n_nodes, 0.);
    std::vector<double> y;
    if (exo_dim >= 2)
        y.resize(n_nodes);
    std::vector<double> z;
    if (exo_dim >= 3)
        z.resize(n_nodes);

    auto n_all_cells = mesh.get_num_all_cells();
    std::size_t i = 0;
    for (auto & cid : mesh.get_cell_range()) {
        auto conn = mesh.get_connectivity(cid);
        for (std::size_t j = 0; j < conn.size(); ++j, ++i) {
            Int ni = conn[j] - n_all_cells;
            x[i] = xyz[ni * dim + 0];
            if (dim >= 2)
                y[i] = xyz[ni * dim + 1];
            if (dim >= 3)
                z[i] = xyz[ni * dim + 2];
        }
    }

    if (exo_dim == 1)
        f.write_coords(x);
    else if (exo_dim == 2)
        f.write_coords(x, y);
    else if (exo_dim == 3)
        f.write_coords(x, y, z);

    coord.restore_array(xyz);

    f.write_coord_names();
}

template <typename Policy>
void
write_elements(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    std::vector<std::string> block_names;

    auto n_cells_sets = mesh.get_num_cell_sets();

    if (n_cells_sets > 1) {
        block_names.resize(n_cells_sets);

        auto cell_sets_label = mesh.get_label("Cell Sets");
        auto cell_set_is = cell_sets_label.get_value_index_set();
        auto cell_set_idx = cell_set_is.borrow_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cell_stratum = cell_sets_label.get_stratum(cell_set_idx[i]);
            auto cells = cell_stratum.borrow_indices();
            auto polytope_type = mesh.get_cell_type(cells[0]);
            Policy::write_block(f,
                                mesh,
                                (int) cell_set_idx[i],
                                polytope_type,
                                cells.size(),
                                cells.data());
            auto name = mesh.get_cell_set_name(cell_set_idx[i]);
            block_names[i] = name.value();
        }
    }
    else {
        auto depth_label = mesh.get_depth_label();
        auto dim = mesh.get_dimension();
        auto cell_stratum = depth_label.get_stratum(dim);
        auto cells = cell_stratum.borrow_indices();
        auto polytope_type = mesh.get_cell_type(cells[0]);
        auto n_elems = mesh.get_num_cells();
        Policy::write_block(f, mesh, SINGLE_BLK_ID, polytope_type, n_elems, cells.data());
    }

    f.write_block_names(block_names);
}

void
write_node_sets(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    if (!mesh.has_label("Vertex Sets"))
        return;

    std::vector<std::string> ns_names;

    auto vertex_sets_label = mesh.get_label("Vertex Sets");

    Int n_node_sets = mesh.get_num_vertex_sets();
    ns_names.resize(n_node_sets);

    auto elem_range = mesh.get_cell_range();
    Int n_elems_in_block = elem_range.size();

    auto vertex_set_is = vertex_sets_label.get_value_index_set();
    auto vertex_set_idx = vertex_set_is.borrow_indices();
    for (Int i = 0; i < n_node_sets; ++i) {
        auto vertex_stratum = vertex_sets_label.get_stratum(vertex_set_idx[i]);
        auto vertices = vertex_stratum.borrow_indices();
        Int n_nodes_in_set = vertices.size();
        std::vector<int> node_set(n_nodes_in_set);
        for (Int j = 0; j < n_nodes_in_set; ++j)
            node_set[j] = (int) (vertices[j] - n_elems_in_block + 1);
        f.write_node_set(vertex_set_idx[i], node_set);

        ns_names[i] = mesh.get_vertex_set_name(vertex_set_idx[i]).value();
    }

    f.write_node_set_names(ns_names);
}

void
write_face_sets(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    if (!mesh.has_label("Face Sets"))
        return;

    auto dm = mesh.get_dm();
    std::vector<std::string> fs_names;

    auto face_sets_label = mesh.get_label("Face Sets");

    Int n_side_sets = mesh.get_num_face_sets();
    fs_names.resize(n_side_sets);

    auto face_set_is = face_sets_label.get_value_index_set();
    auto face_set_idx = face_set_is.borrow_indices();
    for (Int fs = 0; fs < n_side_sets; ++fs) {
        auto face_stratum = face_sets_label.get_stratum(face_set_idx[fs]);
        auto faces = face_stratum.borrow_indices();
        Int face_set_size = faces.size();
        std::vector<int> elem_list(face_set_size);
        std::vector<int> side_list(face_set_size);
        for (Int i = 0; i < face_set_size; ++i) {
            // Element
            Int num_points;
            Int * points = nullptr;
            PETSC_CHECK(
                DMPlexGetTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            Int el = points[2];
            auto polytope_type = mesh.get_cell_type(el);
            auto side_ordering = get_elem_side_ordering(polytope_type);
            elem_list[i] = (int) (el + 1);

            PETSC_CHECK(
                DMPlexRestoreTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            // Side
            points = nullptr;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));

            for (Int j = 1; j < num_points; ++j) {
                if (points[j * 2] == faces[i]) {
                    side_list[i] = (int) side_ordering[j - 1];
                    break;
                }
            }

            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));
        }
        f.write_side_set(face_set_idx[fs], elem_list, side_list);

        fs_names[fs] = mesh.get_face_set_name(face_set_idx[fs]).value();
    }

    f.write_side_set_names(fs_names);
}

// High-level mesh writing

template <typename Policy>
void write_mesh_impl(exodusIIcpp::File & f, const UnstructuredMesh & mesh);

template <>
void
write_mesh_impl<ContinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    write_mesh_init<ContinousPolicy>(f, mesh);
    write_coords<ContinousPolicy>(f, mesh);
    write_elements<ContinousPolicy>(f, mesh);
    write_node_sets(f, mesh);
    write_face_sets(f, mesh);
}

template <>
void
write_mesh_impl<DiscontinousPolicy>(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    CALL_STACK_MSG();
    write_mesh_init<DiscontinousPolicy>(f, mesh);
    write_coords<DiscontinousPolicy>(f, mesh);
    write_elements<DiscontinousPolicy>(f, mesh);
}

void
write_mesh(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    write_mesh_impl<ContinousPolicy>(f, mesh);
}

void
write_mesh_discontinuous(exodusIIcpp::File & f, const UnstructuredMesh & mesh)
{
    write_mesh_impl<DiscontinousPolicy>(f, mesh);
}

// Variable declaration

template <typename Policy>
std::vector<String>
get_var_names_impl(DiscreteProblemInterface & dpi, FieldID fid)
{
    CALL_STACK_MSG();
    auto name = Policy::get_name(dpi, fid).value();
    Int nc = Policy::get_num_components(dpi, fid).value();
    if (nc == 1)
        return { name };
    else {
        std::vector<String> var_names;
        for (Int c = 0; c < nc; ++c) {
            auto comp_name = Policy::get_component_name(dpi, fid, c).value();
            String s;
            if (comp_name.length() == 0)
                s = fmt::format("{}_{}", name, c);
            else
                s = fmt::format("{}", comp_name);
            var_names.push_back(s);
        }
        return var_names;
    }
}

std::vector<String>
get_var_names(DiscreteProblemInterface & dpi, FieldID fid)
{
    CALL_STACK_MSG();
    return get_var_names_impl<PrimaryFieldPolicy>(dpi, fid);
}

std::vector<String>
get_aux_var_names(DiscreteProblemInterface & dpi, FieldID fid)
{
    CALL_STACK_MSG();
    return get_var_names_impl<AuxFieldPolicy>(dpi, fid);
}

// Solution writing

template <typename Policy>
void
write_nodal_field_values(exodusIIcpp::File & f,
                         const DiscreteProblemInterface & dpi,
                         int step_num,
                         Real time,
                         FieldID fid,
                         int exo_var_id)
{
    CALL_STACK_MSG();
    auto sln = Policy::get_solution(dpi);
    auto sln_vals = sln.borrow_array_read();
    auto mesh = dpi.get_mesh();
    auto n_all_elems = mesh->get_num_all_cells();
    auto nc = Policy::get_num_components(dpi, fid).value();

    for (auto n : mesh->get_vertex_range()) {
        auto offset = Policy::get_dof(dpi, n, fid);
        for (Int c = 0; c < nc; ++c) {
            int exo_idx = n - n_all_elems + 1;
            f.write_partial_nodal_var(step_num, exo_var_id + c, 1, exo_idx, sln_vals[offset + c]);
        }
    }
}

void
write_field_values(exodusIIcpp::File & f,
                   const DiscreteProblemInterface & dpi,
                   int step_num,
                   Real time,
                   FieldID fid,
                   int exo_var_id)
{
    CALL_STACK_MSG();
    write_nodal_field_values<PrimaryFieldPolicy>(f, dpi, step_num, time, fid, exo_var_id);
}

void
write_aux_field_values(exodusIIcpp::File & f,
                       const DiscreteProblemInterface & dpi,
                       int step_num,
                       Real time,
                       FieldID fid,
                       int exo_var_id)
{
    CALL_STACK_MSG();
    write_nodal_field_values<AuxFieldPolicy>(f, dpi, step_num, time, fid, exo_var_id);
}

template <typename Policy>
void
write_nodal_field_values(exodusIIcpp::File & f,
                         const DGProblemInterface & dgpi,
                         int step_num,
                         Real time,
                         FieldID fid,
                         int exo_var_id)
{
    CALL_STACK_MSG();
    auto sln = Policy::get_solution(dgpi);
    auto sln_vals = sln.borrow_array_read();
    auto mesh = dgpi.get_mesh();
    auto nc = Policy::get_num_components(dgpi, fid).value();
    int n_nodes_per_elem =
        UnstructuredMesh::get_num_cell_nodes(mesh->get_cell_type(*mesh->get_cell_range().begin()));

    for (auto cid : mesh->get_cell_range()) {
        for (Int c = 0; c < nc; ++c) {
            for (int lni = 0; lni < n_nodes_per_elem; ++lni) {
                int exo_idx = (cid * n_nodes_per_elem + lni) + 1;
                auto offset = Policy::get_dof(dgpi, cid, lni, fid);
                f.write_partial_nodal_var(step_num,
                                          exo_var_id + c,
                                          1,
                                          exo_idx,
                                          sln_vals[offset + c]);
            }
        }
    }
}

void
write_field_values(exodusIIcpp::File & f,
                   const DGProblemInterface & dgpi,
                   int step_num,
                   Real time,
                   FieldID fid,
                   int exo_var_id)
{
    CALL_STACK_MSG();
    write_nodal_field_values<PrimaryFieldPolicy>(f, dgpi, step_num, time, fid, exo_var_id);
}

void
write_aux_field_values(exodusIIcpp::File & f,
                       const DGProblemInterface & dgpi,
                       int step_num,
                       Real time,
                       FieldID fid,
                       int exo_var_id)
{
    CALL_STACK_MSG();
    write_nodal_field_values<AuxFieldPolicy>(f, dgpi, step_num, time, fid, exo_var_id);
}

template <typename Policy>
void
write_elem_field_values(exodusIIcpp::File & f,
                        DiscreteProblemInterface & dpi,
                        int step_num,
                        Real time,
                        FieldID fid,
                        int exo_var_id,
                        int blk_id)
{
    CALL_STACK_MSG();
    auto sln = Policy::get_solution(dpi);
    auto sln_vals = sln.borrow_array_read();
    auto mesh = dpi.get_mesh();
    auto nc = Policy::get_num_components(dpi, fid).value();

    for (auto cell : mesh->get_cell_range()) {
        auto offset = Policy::get_dof(dpi, cell, fid);
        for (Int c = 0; c < nc; ++c) {
            int exo_idx = (int) (cell + 1);
            f.write_partial_elem_var(step_num,
                                     exo_var_id + c,
                                     blk_id,
                                     exo_idx,
                                     sln_vals[offset + c]);
        }
    }
}

template <typename Policy>
void
write_elem_field_values(exodusIIcpp::File & f,
                        DiscreteProblemInterface & dpi,
                        int step_num,
                        Real time,
                        FieldID fid,
                        int exo_var_id,
                        int blk_id,
                        Span<const Int> cells)
{
    CALL_STACK_MSG();
    auto sln = Policy::get_solution(dpi);
    auto sln_vals = sln.borrow_array_read();
    auto nc = Policy::get_num_components(dpi, fid).value();

    for (auto i : make_range(cells.size())) {
        auto elem_id = cells[i];
        auto offset = Policy::get_dof(dpi, elem_id, fid);
        for (Int c = 0; c < nc; ++c) {
            auto exo_idx = (int) (i + 1);
            f.write_partial_elem_var(step_num,
                                     exo_var_id + c,
                                     blk_id,
                                     exo_idx,
                                     sln_vals[offset + c]);
        }
    }
}

template <typename Policy>
void
write_block_elem_values(exodusIIcpp::File & f,
                        DiscreteProblemInterface & dpi,
                        int step_num,
                        Real time,
                        FieldID fid,
                        int exo_var_id)
{
    CALL_STACK_MSG();
    auto mesh = dpi.get_mesh();

    Int n_cells_sets = mesh->get_num_cell_sets();
    if (n_cells_sets > 1) {
        auto cell_sets_label = mesh->get_label("Cell Sets");
        auto cell_set_is = cell_sets_label.get_value_index_set();
        auto cell_set_idx = cell_set_is.borrow_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cell_stratum = cell_sets_label.get_stratum(cell_set_idx[i]);
            auto cells = cell_stratum.borrow_indices();
            write_elem_field_values<Policy>(f,
                                            dpi,
                                            step_num,
                                            time,
                                            fid,
                                            exo_var_id,
                                            (int) cell_set_idx[i],
                                            Span(cells.data(), cells.size()));
        }
    }
    else
        write_elem_field_values<Policy>(f, dpi, step_num, time, fid, exo_var_id, SINGLE_BLK_ID);
}

void
write_elemental_field_values(exodusIIcpp::File & f,
                             DiscreteProblemInterface & dpi,
                             int step_num,
                             Real time,
                             FieldID fid,
                             int exo_var_id)
{
    CALL_STACK_MSG();
    write_block_elem_values<PrimaryFieldPolicy>(f, dpi, step_num, time, fid, exo_var_id);
}

void
write_aux_elemental_field_values(exodusIIcpp::File & f,
                                 DiscreteProblemInterface & dpi,
                                 int step_num,
                                 Real time,
                                 FieldID fid,
                                 int exo_var_id)
{
    CALL_STACK_MSG();
    write_block_elem_values<AuxFieldPolicy>(f, dpi, step_num, time, fid, exo_var_id);
}

} // namespace io
} // namespace godzilla
