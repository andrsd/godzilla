//  SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
//  SPDX-License-Identifier: MIT

#include "godzilla/FileMesh.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Types.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include "petscdm.h"
#include "petsc/private/dmimpl.h"

using namespace godzilla;

namespace {

DMPolytopeType
get_cell_type(const std::string & elem_type)
{
    if (elem_type == "EDGE2")
        return DM_POLYTOPE_SEGMENT;
    else if (elem_type == "TRI" || elem_type == "TRI3")
        return DM_POLYTOPE_TRIANGLE;
    else if (elem_type == "QUAD" || elem_type == "QUAD4" || elem_type == "SHELL4")
        return DM_POLYTOPE_QUADRILATERAL;
    else if (elem_type == "TETRA" || elem_type == "TET4")
        return DM_POLYTOPE_TETRAHEDRON;
    else if (elem_type == "WEDGE")
        return DM_POLYTOPE_TRI_PRISM;
    else if (elem_type == "HEX" || elem_type == "HEX8" || elem_type == "HEXAHEDRON")
        return DM_POLYTOPE_HEXAHEDRON;
    else
        throw Exception(fmt::format("Unrecognized element type {}", elem_type));
}

} // namespace

// This is a rewrite of `DMPlexCreateExodus` from PETSc (`plexexdosuii.c`) using exodusIIcpp
// and C++ constructs. Plus, it adds some godzilla-specific stuff like mapping names to labels, etc.
godzilla::UnstructuredMesh *
FileMesh::create_from_exodus()
{
    CALL_STACK_MSG();

    auto comm = get_comm();
    auto rank = comm.rank();

    auto m = new UnstructuredMesh(comm);

    try {
        Int dim = 0;
        Int dim_embed = 0;
        Int n_cells = 0;
        Int n_vertices = 0;
        Int n_side_sets = 0;

        exodusIIcpp::File f;
        if (rank == 0) {
            f.open(get_file_name());
            f.init();
            dim_embed = f.get_dim();
            n_cells = f.get_num_elements();
            n_vertices = f.get_num_nodes();
            n_side_sets = f.get_num_side_sets();
            m->set_chart(0, n_cells + n_vertices);
            // We do not want this label automatically computed, instead we compute it here
            m->create_label("celltype");

            f.read_blocks();
            int n_elem_blocks = f.get_num_element_blocks();
            std::vector<int> cs_order(n_elem_blocks);
            int num_hybrid = 0;
            for (int i = 0; i < n_elem_blocks; i++) {
                auto eb = f.get_element_block(i);
                DMPolytopeType ct = ::get_cell_type(eb.get_element_type());
                dim = std::max(dim, DMPolytopeTypeGetDim(ct));
                if (ct == DM_POLYTOPE_TRI_PRISM) {
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
                DMPolytopeType ct = ::get_cell_type(eb.get_element_type());
                for (int j = 0; j < eb.get_num_elements(); ++j, ++cell) {
                    m->set_cone_size(cell, eb.get_num_nodes_per_element());
                    m->set_cell_type(cell, ct);
                }
            }
            for (Int vertex = n_cells; vertex < n_cells + n_vertices; ++vertex)
                m->set_cell_type(vertex, DM_POLYTOPE_POINT);
            m->set_up();

            // process element blocks
            m->create_label("Cell Sets");
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
                m->set_cell_set_name(id, name);

                auto n_blk_elems = eb.get_num_elements();
                auto n_elem_nodes = eb.get_num_nodes_per_element();
                auto & connect = eb.get_connectivity();
                // EXO uses Fortran-based indexing, DMPlex uses C-style and numbers cells first then
                // vertices.
                std::vector<Int> cone(n_elem_nodes);
                for (Int j = 0, vertex = 0; j < n_blk_elems; ++j, ++cell) {
                    for (Int k = 0; k < n_elem_nodes; ++k, ++vertex)
                        cone[k] = connect[vertex] + n_cells - 1;
                    DMPolytopeType ct = m->get_cell_type(cell);
                    PETSC_CHECK(DMPlexInvertCell(ct, cone.data()));
                    m->set_cone(cell, cone);
                    cell_sets.set_value(cell, id);
                    block_label.set_value(cell, id);
                }
            }
        }

        // broadcast dimensions to all ranks
        Int ints[] = { dim, dim_embed };
        comm.broadcast(ints, 2, 0);
        m->set_dimension(ints[0]);
        m->set_coordinate_dim(ints[1]);
        dim = ints[0];
        dim_embed = ints[1];

        m->symmetrize();
        m->stratify();
        m->interpolate();

        // TODO: create vertex sets

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
        if ((rank == 0) && (n_side_sets > 0)) {
            m->create_label("Face Sets");
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
                m->set_face_set_name(id, name);

                std::vector<int> node_count_list;
                std::vector<int> node_list;
                f.get_side_set_node_list(id, node_count_list, node_list);

                // quads have 4 nodes which is the maximum on a side that we can have
                const Int MAX_FACE_VERTICES = 4;
                auto num_side_in_set = side_set.get_size();
                for (Int j = 0, k = 0; j < num_side_in_set; ++j) {
                    Int face_size = node_count_list[j];
                    if (face_size > MAX_FACE_VERTICES)
                        throw Exception(
                            fmt::format("ExodusII side cannot have more than {} vertices.",
                                        MAX_FACE_VERTICES));

                    std::vector<Int> face_nodes(MAX_FACE_VERTICES);
                    for (Int l = 0; l < face_size; ++l, ++k)
                        face_nodes[l] = node_list[k] + n_cells - 1;

                    auto faces = m->get_full_join(face_nodes);
                    if (faces.size() != 1)
                        throw Exception(
                            fmt::format("Invalid ExodusII side {} in set {} maps to {} faces.",
                                        j,
                                        i,
                                        faces.size()));

                    face_sets.set_value(faces[0], id);
                    face_set_label.set_value(faces[0], id);
                }
            }
        }
    }
    catch (exodusIIcpp::Exception & e) {
        log_error(fmt::format(e.what()));
    }
    catch (godzilla::Exception & e) {
        log_error(fmt::format(e.what()));
    }

    return m;
}
