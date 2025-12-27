// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FileMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Types.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include "petscdm.h"
#include "petsc/private/dmimpl.h"
#include "petscdmplex.h"

namespace godzilla {

namespace {

PolytopeType
get_cell_type_ex2(String elem_type)
{
    if (elem_type == "EDGE2" || elem_type == "BAR2" || elem_type == "BEAM" || elem_type == "BEAM2")
        return PolytopeType::SEGMENT;
    else if (elem_type == "TRI" || elem_type == "TRIANGLE" || elem_type == "TRI3")
        return PolytopeType::TRIANGLE;
    else if (elem_type == "QUAD" || elem_type == "QUAD4" || elem_type == "SHELL" ||
             elem_type == "SHELL4")
        return PolytopeType::QUADRILATERAL;
    else if (elem_type == "TETRA" || elem_type == "TET4" || elem_type == "TETRA4")
        return PolytopeType::TETRAHEDRON;
    else if (elem_type == "WEDGE")
        return PolytopeType::TRI_PRISM;
    else if (elem_type == "HEX" || elem_type == "HEX8" || elem_type == "HEXAHEDRON")
        return PolytopeType::HEXAHEDRON;
    else
        throw Exception("Unrecognized element type {}", elem_type);
}

} // namespace

// This is a rewrite of `DMPlexCreateExodus` from PETSc (`plexexodusii.c`) using exodusIIcpp
// and C++ constructs. Plus, it adds some godzilla-specific stuff like mapping names to labels, etc.
Qtr<UnstructuredMesh>
FileMesh::create_from_exodus()
{
    CALL_STACK_MSG();

    auto comm = get_comm();
    auto rank = comm.rank();

    auto m = Qtr<UnstructuredMesh>::alloc(comm);

    try {
        Int dim = 0;
        Int dim_embed = 0;
        Int n_cells = 0;
        Int n_vertices = 0;
        Int n_side_sets = 0;
        Int n_node_sets = 0;
        std::map<Int, String> cell_set_names;

        exodusIIcpp::File f;
        if (rank == 0) {
            f.open(get_file_name());
            f.init();
            dim_embed = f.get_dim();
            n_cells = f.get_num_elements();
            n_vertices = f.get_num_nodes();
            n_side_sets = f.get_num_side_sets();
            n_node_sets = f.get_num_node_sets();
        }
        m->set_chart(0, n_cells + n_vertices);
        // We do not want this label automatically computed, instead we compute it here
        m->create_label("celltype");
        // Create Cell/Face/Vertex Sets labels on all processes
        m->create_label("Cell Sets");
        m->create_label("Face Sets");
        m->create_label("Vertex Sets");

        if (rank == 0) {
            f.read_blocks();
            int n_elem_blocks = f.get_num_element_blocks();
            std::vector<int> cs_order(n_elem_blocks);
            int num_hybrid = 0;
            for (int i = 0; i < n_elem_blocks; ++i) {
                auto eb = f.get_element_block(i);
                auto ct = get_cell_type_ex2(eb.get_element_type());
                dim = math::max(dim, UnstructuredMesh::get_polytope_dim(ct));
                if (ct == PolytopeType::TRI_PRISM) {
                    cs_order[i] = i;
                    ++num_hybrid;
                }
                else {
                    for (int c = i; c > i - num_hybrid; --c)
                        cs_order[c] = cs_order[c - 1];
                    cs_order[i - num_hybrid] = i;
                }
            }
            // Set sizes
            for (Int i = 0, cell = 0; i < n_elem_blocks; ++i) {
                int blk_idx = cs_order[i];
                auto eb = f.get_element_block(blk_idx);
                auto ct = get_cell_type_ex2(eb.get_element_type());
                for (int j = 0; j < eb.get_num_elements(); ++j, ++cell) {
                    m->set_cone_size(cell, eb.get_num_nodes_per_element());
                    m->set_cell_type(cell, ct);
                }
            }
            for (Int vertex = n_cells; vertex < n_cells + n_vertices; ++vertex)
                m->set_cell_type(vertex, PolytopeType::POINT);
            m->set_up();

            // process element blocks
            auto cell_sets = m->get_label("Cell Sets");

            for (Int i = 0, cell = 0; i < n_elem_blocks; ++i) {
                int blk_idx = cs_order[i];
                auto & eb = f.get_element_block(blk_idx);
                auto id = eb.get_id();
                auto name = eb.get_name();
                if (name.empty())
                    name = fmt::format("{}", id);

                m->create_label(name);
                auto block_label = m->get_label(name);
                cell_set_names[id] = name;

                auto n_blk_elems = eb.get_num_elements();
                auto n_elem_nodes = eb.get_num_nodes_per_element();
                auto & connect = eb.get_connectivity();
                // EXO uses Fortran-based indexing, DMPlex uses C-style and numbers cells first then
                // vertices.
                std::vector<Int> cone(n_elem_nodes);
                for (Int j = 0, vertex = 0; j < n_blk_elems; ++j, ++cell) {
                    for (Int k = 0; k < n_elem_nodes; ++k, ++vertex)
                        cone[k] = connect[vertex] + n_cells - 1;
                    auto ct = m->get_cell_type(cell);
                    UnstructuredMesh::invert_cell(ct, cone);
                    m->set_cone(cell, cone);
                    cell_sets.set_value(cell, id);
                    block_label.set_value(cell, id);
                }
            }
        }
        comm.broadcast(cell_set_names, 0);
        for (const auto & [id, name] : cell_set_names)
            m->set_cell_set_name(id, name);

        // broadcast dimensions to all ranks
        Int ints[] = { dim, dim_embed };
        comm.broadcast(ints, 2, 0);
        m->set_dimension(Dimension::from_int(ints[0]));
        m->set_coordinate_dim(Dimension::from_int(ints[1]));
        dim = ints[0];
        dim_embed = ints[1];

        m->symmetrize();
        m->stratify();
        m->interpolate();

        // create vertex sets
        std::map<Int, String> node_set_names;
        if (rank == 0 && (n_node_sets > 0)) {
            auto vertex_sets = m->get_label("Vertex Sets");
            f.read_node_sets();
            for (auto & node_set : f.get_node_sets()) {
                auto id = node_set.get_id();
                auto name = node_set.get_name();
                if (name.empty())
                    name = fmt::format("{}", id);

                m->create_label(name);
                auto vertex_set_label = m->get_label(name);
                node_set_names[id] = name;

                for (int v = 0; v < node_set.get_size(); ++v) {
                    auto node = node_set.get_node_id(v);
                    auto vertex = node + n_cells - 1;
                    vertex_sets.set_value(vertex, id);
                    vertex_set_label.set_value(vertex, id);
                }
            }
        }
        comm.broadcast(node_set_names, 0);
        for (const auto & [id, name] : node_set_names)
            m->set_vertex_set_name(id, name);

        // Read coordinates
        auto coord_section = m->get_coordinate_section();
        coord_section.set_num_fields(1);
        coord_section.set_num_field_components(0, dim_embed);
        coord_section.set_chart(n_cells, n_cells + n_vertices);

        for (Int vertex = n_cells; vertex < n_cells + n_vertices; ++vertex) {
            coord_section.set_dof(vertex, dim_embed);
            coord_section.set_field_dof(vertex, 0, dim_embed);
        }
        coord_section.set_up();
        auto coord_size = coord_section.get_storage_size();

        Vector coordinates(comm);
        coordinates.set_name("coordinates");
        coordinates.set_sizes(coord_size, PETSC_DETERMINE);
        coordinates.set_block_size(dim_embed);
        coordinates.set_type(VECSTANDARD);
        Scalar * coords = coordinates.get_array();
        if (rank == 0) {
            f.read_coords();
            if (dim_embed > 0) {
                auto & x = f.get_x_coords();
                for (Int i = 0; i < n_vertices; ++i)
                    coords[i * dim_embed + 0] = x[i];
            }
            if (dim_embed > 1) {
                auto & y = f.get_y_coords();
                for (Int i = 0; i < n_vertices; ++i)
                    coords[i * dim_embed + 1] = y[i];
            }
            if (dim_embed > 2) {
                auto & z = f.get_z_coords();
                for (Int i = 0; i < n_vertices; ++i)
                    coords[i * dim_embed + 2] = z[i];
            }
        }
        coordinates.restore_array(coords);
        m->set_coordinates_local(coordinates);
        coordinates.destroy();

        // Create side set labels
        std::map<Int, String> side_set_names;
        if ((rank == 0) && (n_side_sets > 0)) {
            auto face_sets = m->get_label("Face Sets");

            f.read_side_sets();
            auto & side_sets = f.get_side_sets();
            for (int i = 0; i < n_side_sets; ++i) {
                auto & side_set = side_sets[i];
                auto id = side_set.get_id();
                auto name = side_set.get_name();
                if (name.empty())
                    name = fmt::format("{}", id);

                m->create_label(name);
                auto face_set_label = m->get_label(name);
                side_set_names[id] = name;

                std::vector<int> node_count_list;
                std::vector<int> node_list;
                f.get_side_set_node_list(id, node_count_list, node_list);

                // quads have 4 nodes which is the maximum on a side that we can have
                const Int MAX_FACE_VERTICES = 4;
                auto num_side_in_set = side_set.get_size();
                for (Int j = 0, k = 0; j < num_side_in_set; ++j) {
                    Int face_size = node_count_list[j];
                    if (face_size > MAX_FACE_VERTICES)
                        throw Exception("ExodusII side cannot have more than {} vertices.",
                                        MAX_FACE_VERTICES);

                    std::vector<Int> face_nodes(face_size);
                    for (Int l = 0; l < face_size; ++l, ++k)
                        face_nodes[l] = node_list[k] + n_cells - 1;

                    auto faces = m->get_full_join(face_nodes);
                    if (faces.size() != 1)
                        throw Exception("Invalid ExodusII side {} in set {} maps to {} faces.",
                                        j,
                                        i,
                                        faces.size());

                    face_sets.set_value(faces[0], id);
                    face_set_label.set_value(faces[0], id);
                }
            }
        }
        comm.broadcast(side_set_names, 0);
        for (const auto & [id, name] : side_set_names)
            m->set_face_set_name(id, name);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("exodusIIcpp: {}", e.what());
    }

    return m;
}

} // namespace godzilla
