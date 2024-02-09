//  SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
//  SPDX-License-Identifier: MIT

#include "godzilla/FileMesh.h"
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
void
FileMesh::create_from_exodus()
{
    CALL_STACK_MSG();

    auto comm = get_comm();
    auto rank = comm.rank();

    DM dm = nullptr;
    PETSC_CHECK(DMCreate(comm, &dm));
    PETSC_CHECK(DMSetType(dm, DMPLEX));

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
            PETSC_CHECK(DMPlexSetChart(dm, 0, n_cells + n_vertices));
            // We do not want this label automatically computed, instead we compute it here
            PETSC_CHECK(DMCreateLabel(dm, "celltype"));

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
                    PETSC_CHECK(DMPlexSetConeSize(dm, cell, eb.get_num_nodes_per_element()));
                    PETSC_CHECK(DMPlexSetCellType(dm, cell, ct));
                }
            }
            for (Int vertex = n_cells; vertex < n_cells + n_vertices; ++vertex)
                PETSC_CHECK(DMPlexSetCellType(dm, vertex, DM_POLYTOPE_POINT));
            PETSC_CHECK(DMSetUp(dm));

            // process element blocks
            PETSC_CHECK(DMCreateLabel(dm, "Cell Sets"));
            DMLabel cell_sets;
            PETSC_CHECK(DMGetLabel(dm, "Cell Sets", &cell_sets));

            for (Int i = 0, cell = 0; i < n_elem_blocks; ++i) {
                int blk_idx = cs_order[i];
                auto & eb = f.get_element_block(blk_idx);
                auto id = eb.get_id();
                auto name = eb.get_name();
                if (name.empty())
                    name = fmt::format("{}", id);

                PETSC_CHECK(DMCreateLabel(dm, name.c_str()));
                DMLabel block_label;
                PETSC_CHECK(DMGetLabel(dm, name.c_str(), &block_label));
                set_cell_set_name(id, name);

                auto n_blk_elems = eb.get_num_elements();
                auto n_elem_nodes = eb.get_num_nodes_per_element();
                auto & connect = eb.get_connectivity();
                // EXO uses Fortran-based indexing, DMPlex uses C-style and numbers cells first then
                // vertices.
                std::vector<Int> cone(n_elem_nodes);
                for (Int j = 0, vertex = 0; j < n_blk_elems; ++j, ++cell) {
                    for (Int k = 0; k < n_elem_nodes; ++k, ++vertex)
                        cone[k] = connect[vertex] + n_cells - 1;
                    DMPolytopeType ct;
                    PETSC_CHECK(DMPlexGetCellType(dm, cell, &ct));
                    PETSC_CHECK(DMPlexInvertCell(ct, cone.data()));
                    PETSC_CHECK(DMPlexSetCone(dm, cell, cone.data()));
                    PETSC_CHECK(DMLabelSetValue(cell_sets, cell, id));
                    PETSC_CHECK(DMLabelSetValue(block_label, cell, id));
                }
            }
        }

        // broadcast dimensions to all ranks
        Int ints[] = { dim, dim_embed };
        comm.broadcast(ints, 2, 0);
        PETSC_CHECK(DMSetDimension(dm, ints[0]));
        PETSC_CHECK(DMSetCoordinateDim(dm, ints[1]));
        dim = ints[0];
        dim_embed = ints[1];

        PETSC_CHECK(DMPlexSymmetrize(dm));
        PETSC_CHECK(DMPlexStratify(dm));
        DM idm;
        PETSC_CHECK(DMPlexInterpolate(dm, &idm));
        PETSC_CHECK(DMDestroy(&dm));
        dm = idm;

        // TODO: create vertex sets

        // Read coordinates
        PetscSection coord_section;
        Vec coordinates;
        Scalar * coords;
        Int coord_size;
        PETSC_CHECK(DMGetCoordinateSection(dm, &coord_section));
        PETSC_CHECK(PetscSectionSetNumFields(coord_section, 1));
        PETSC_CHECK(PetscSectionSetFieldComponents(coord_section, 0, dim_embed));
        PETSC_CHECK(PetscSectionSetChart(coord_section, n_cells, n_cells + n_vertices));
        for (Int vertex = n_cells; vertex < n_cells + n_vertices; ++vertex) {
            PETSC_CHECK(PetscSectionSetDof(coord_section, vertex, dim_embed));
            PETSC_CHECK(PetscSectionSetFieldDof(coord_section, vertex, 0, dim_embed));
        }
        PETSC_CHECK(PetscSectionSetUp(coord_section));
        PETSC_CHECK(PetscSectionGetStorageSize(coord_section, &coord_size));
        PETSC_CHECK(VecCreate(comm, &coordinates));
        PETSC_CHECK(PetscObjectSetName((PetscObject) coordinates, "coordinates"));
        PETSC_CHECK(VecSetSizes(coordinates, coord_size, PETSC_DETERMINE));
        PETSC_CHECK(VecSetBlockSize(coordinates, dim_embed));
        PETSC_CHECK(VecSetType(coordinates, VECSTANDARD));
        PETSC_CHECK(VecGetArray(coordinates, &coords));
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
        PETSC_CHECK(VecRestoreArray(coordinates, &coords));
        PETSC_CHECK(DMSetCoordinatesLocal(dm, coordinates));
        PETSC_CHECK(VecDestroy(&coordinates));

        // Create side set labels
        if ((rank == 0) && (n_side_sets > 0)) {
            PETSC_CHECK(DMCreateLabel(dm, "Face Sets"));
            DMLabel face_sets;
            PETSC_CHECK(DMGetLabel(dm, "Face Sets", &face_sets));

            f.read_side_sets();
            auto & side_sets = f.get_side_sets();
            for (int i = 0; i < n_side_sets; ++i) {
                auto & side_set = side_sets[i];
                auto id = side_set.get_id();
                auto name = side_set.get_name();
                if (name.empty())
                    name = fmt::format("{}", id);

                PETSC_CHECK(DMCreateLabel(dm, name.c_str()));
                DMLabel face_set_label;
                PETSC_CHECK(DMGetLabel(dm, name.c_str(), &face_set_label));
                set_face_set_name(id, name);

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

                    Int face_nodes[MAX_FACE_VERTICES];
                    for (Int l = 0; l < face_size; ++l, ++k)
                        face_nodes[l] = node_list[k] + n_cells - 1;

                    Int n_faces;
                    const Int * faces = nullptr;
                    PETSC_CHECK(DMPlexGetFullJoin(dm, face_size, face_nodes, &n_faces, &faces));
                    if (n_faces != 1)
                        throw Exception(
                            fmt::format("Invalid ExodusII side {} in set {} maps to {} faces.",
                                        j,
                                        i,
                                        n_faces));
                    PETSC_CHECK(DMLabelSetValue(face_sets, faces[0], id));
                    PETSC_CHECK(DMLabelSetValue(face_set_label, faces[0], id));
                    PETSC_CHECK(DMPlexRestoreJoin(dm, face_size, face_nodes, &n_faces, &faces));
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

    set_dm(dm);
}
