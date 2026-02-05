// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/DGProblemInterface.h"
#include "godzilla/Ref.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Exception.h"
#include "fmt/format.h"
#include "fmt/chrono.h"
#include <set>

namespace godzilla {

const int ExodusIIOutput::SINGLE_BLK_ID = 0;

const char *
ExodusIIOutput::get_elem_type(PolytopeType elem_type)
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

const Int *
ExodusIIOutput::get_elem_node_ordering(PolytopeType elem_type)
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

const Int *
ExodusIIOutput::get_elem_side_ordering(PolytopeType elem_type)
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

Parameters
ExodusIIOutput::parameters()
{
    auto params = FileOutput::parameters();
    params.add_param<std::vector<String>>(
        "variables",
        std::vector<String> {},
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const Parameters & pars) :
    FileOutput(pars),
    mesh(get_discrete_problem_interface()->get_mesh()),
    cont(false),
    discont(false),
    variable_names(pars.get<std::vector<String>>("variables"), {}),
    dgpi(try_dynamic_ref_cast<DGProblemInterface>(get_problem())),
    step_num(1),
    mesh_stored(false)
{
    CALL_STACK_MSG();
    if (this->dgpi.has_value())
        this->discont = true;
    else
        this->cont = true;
}

ExodusIIOutput::~ExodusIIOutput()
{
    CALL_STACK_MSG();
    if (this->exo)
        this->exo->close();
}

String
ExodusIIOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    return { "exo" };
}

void
ExodusIIOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();

    auto dpi = get_discrete_problem_interface();
    auto flds = dpi->get_field_names();
    auto aux_flds = dpi->get_aux_field_names();
    auto & pps = get_problem()->get_postprocessor_names();

    if (this->variable_names.empty()) {
        this->field_var_names = flds;
        this->aux_field_var_names = aux_flds;
        for (auto & name : pps)
            this->global_var_names.push_back(name);
    }
    else {
        std::set<String> field_names(flds.begin(), flds.end());
        std::set<String> aux_field_names(aux_flds.begin(), aux_flds.end());
        std::set<String> pp_names(pps.begin(), pps.end());

        for (auto & name : this->variable_names) {
            if (field_names.count(name) == 1)
                this->field_var_names.push_back(name);
            else if (aux_field_names.count(name) == 1)
                this->aux_field_var_names.push_back(name);
            else if (pp_names.count(name) == 1)
                this->global_var_names.push_back(name);
            else
                error("Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                      name);
        }
    }
}

void
ExodusIIOutput::output_mesh()
{
    CALL_STACK_MSG();
    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        write_info();
        write_mesh();
    }
}

void
ExodusIIOutput::output_step()
{
    CALL_STACK_MSG();
    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        write_info();
        write_mesh();
        write_all_variable_names();
    }

    write_variables();
    ++this->step_num;
}

void
ExodusIIOutput::open_file()
{
    CALL_STACK_MSG();
    this->exo =
        Qtr<exodusIIcpp::File>::alloc(get_file_name().c_str(), exodusIIcpp::FileAccess::WRITE);
    if (!this->exo->is_opened())
        throw Exception("Could not open file '{}' for writing.", get_file_name());
}

void
ExodusIIOutput::write_mesh()
{
    CALL_STACK_MSG();
    if (cont) {
        write_mesh_continuous();
        write_node_sets();
        write_face_sets();
    }
    else if (discont) {
        write_mesh_discontinuous();
        // TODO: write node sets
        // TODO: write side sets
    }
    this->mesh_stored = true;
}

void
ExodusIIOutput::write_mesh_continuous()
{
    CALL_STACK_MSG();
    int n_nodes = (int) this->mesh->get_num_vertices();
    int n_elems = (int) this->mesh->get_num_cells();

    // number of element blocks
    int n_elem_blk = (int) this->mesh->get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int n_node_sets = (int) this->mesh->get_num_vertex_sets();
    int n_side_sets = (int) this->mesh->get_num_face_sets();

    int exo_dim = this->mesh->get_dimension();
    this->exo->init("", exo_dim, n_nodes, n_elems, n_elem_blk, n_node_sets, n_side_sets);

    write_coords_continuous(exo_dim);
    write_elements();
}

void
ExodusIIOutput::write_mesh_discontinuous()
{
    CALL_STACK_MSG();
    int n_elems = (int) this->mesh->get_num_cells();
    int n_nodes_per_elem = get_num_nodes_per_element();
    int n_nodes = (int) n_elems * n_nodes_per_elem;

    // number of element blocks
    int n_elem_blk = (int) this->mesh->get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int exo_dim = this->mesh->get_dimension();
    this->exo->init("", exo_dim, n_nodes, n_elems, n_elem_blk, 0, 0);

    write_coords_discontinuous(exo_dim);
    write_elements();
}

void
ExodusIIOutput::write_coords_continuous(int exo_dim)
{
    CALL_STACK_MSG();
    int dim = this->mesh->get_dimension();
    Vector coord = this->mesh->get_coordinates_local();
    Int coord_size = coord.get_size();
    Scalar * xyz = coord.get_array();

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
        this->exo->write_coords(x);
    else if (exo_dim == 2)
        this->exo->write_coords(x, y);
    else if (exo_dim == 3)
        this->exo->write_coords(x, y, z);

    coord.restore_array(xyz);

    this->exo->write_coord_names();
}

void
ExodusIIOutput::write_coords_discontinuous(int exo_dim)
{
    CALL_STACK_MSG();
    int dim = this->mesh->get_dimension();
    Vector coord = this->mesh->get_coordinates_local();
    Scalar * xyz = coord.get_array();

    int n_elems = (int) this->mesh->get_num_cells();
    int n_nodes_per_elem = get_num_nodes_per_element();
    int n_nodes = n_elems * n_nodes_per_elem;

    std::vector<double> x(n_nodes, 0.);
    std::vector<double> y;
    if (exo_dim >= 2)
        y.resize(n_nodes);
    std::vector<double> z;
    if (exo_dim >= 3)
        z.resize(n_nodes);

    auto n_all_cells = this->mesh->get_num_all_cells();
    std::size_t i = 0;
    for (auto & cid : this->mesh->get_cell_range()) {
        auto conn = this->mesh->get_connectivity(cid);
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
        this->exo->write_coords(x);
    else if (exo_dim == 2)
        this->exo->write_coords(x, y);
    else if (exo_dim == 3)
        this->exo->write_coords(x, y, z);

    coord.restore_array(xyz);

    this->exo->write_coord_names();
}

void
ExodusIIOutput::write_elements()
{
    CALL_STACK_MSG();
    std::vector<std::string> block_names;

    Int n_cells_sets = this->mesh->get_num_cell_sets();

    if (n_cells_sets > 1) {
        block_names.resize(n_cells_sets);

        auto cell_sets_label = this->mesh->get_label("Cell Sets");
        auto cell_set_is = cell_sets_label.get_value_index_set();
        auto cell_set_idx = cell_set_is.borrow_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cell_stratum = cell_sets_label.get_stratum(cell_set_idx[i]);
            auto cells = cell_stratum.borrow_indices();
            auto polytope_type = this->mesh->get_cell_type(cells[0]);
            if (this->cont)
                write_block_connectivity_continuous((int) cell_set_idx[i],
                                                    polytope_type,
                                                    cells.size(),
                                                    cells.data());
            else if (this->discont)
                write_block_connectivity_discontinuous((int) cell_set_idx[i],
                                                       polytope_type,
                                                       cells.size(),
                                                       cells.data());

            auto name = this->mesh->get_cell_set_name(cell_set_idx[i]);
            block_names[i] = name.value();
        }
    }
    else {
        auto depth_label = this->mesh->get_depth_label();
        auto dim = this->mesh->get_dimension();
        auto cell_stratum = depth_label.get_stratum(dim);
        auto cells = cell_stratum.borrow_indices();
        auto polytope_type = this->mesh->get_cell_type(cells[0]);
        auto n_elems = this->mesh->get_num_cells();
        if (this->cont)
            write_block_connectivity_continuous(SINGLE_BLK_ID,
                                                polytope_type,
                                                n_elems,
                                                cells.data());
        else if (this->discont)
            write_block_connectivity_discontinuous(SINGLE_BLK_ID,
                                                   polytope_type,
                                                   n_elems,
                                                   cells.data());
    }

    this->exo->write_block_names(block_names);
}

void
ExodusIIOutput::write_node_sets()
{
    CALL_STACK_MSG();
    if (!this->mesh->has_label("Vertex Sets"))
        return;

    std::vector<std::string> ns_names;

    auto vertex_sets_label = this->mesh->get_label("Vertex Sets");

    Int n_node_sets = this->mesh->get_num_vertex_sets();
    ns_names.resize(n_node_sets);

    auto elem_range = this->mesh->get_cell_range();
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
        this->exo->write_node_set(vertex_set_idx[i], node_set);

        ns_names[i] = this->mesh->get_vertex_set_name(vertex_set_idx[i]).value();
    }

    this->exo->write_node_set_names(ns_names);
}

void
ExodusIIOutput::write_face_sets()
{
    CALL_STACK_MSG();
    if (!this->mesh->has_label("Face Sets"))
        return;

    auto dm = this->mesh->get_dm();
    std::vector<std::string> fs_names;

    auto face_sets_label = this->mesh->get_label("Face Sets");

    Int n_side_sets = this->mesh->get_num_face_sets();
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
            auto polytope_type = this->mesh->get_cell_type(el);
            const Int * side_ordering = get_elem_side_ordering(polytope_type);
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
        this->exo->write_side_set(face_set_idx[fs], elem_list, side_list);

        fs_names[fs] = this->mesh->get_face_set_name(face_set_idx[fs]).value();
    }

    this->exo->write_side_set_names(fs_names);
}

void
ExodusIIOutput::write_all_variable_names()
{
    CALL_STACK_MSG();

    auto dpi = get_discrete_problem_interface();
    this->nodal_var_fids.clear();
    this->nodal_aux_var_fids.clear();
    this->elem_var_fids.clear();
    this->elem_aux_var_fids.clear();
    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names) {
        auto fid = dpi->get_field_id(name).value();
        auto order = dpi->get_field_order(fid).value();
        if (order == 0) {
            add_var_names(fid, elem_var_names);
            this->elem_var_fids.push_back(fid);
        }
        else {
            add_var_names(fid, nodal_var_names);
            this->nodal_var_fids.push_back(fid);
        }
    }
    for (auto & name : this->aux_field_var_names) {
        auto fid = dpi->get_aux_field_id(name).value();
        auto order = dpi->get_aux_field_order(fid).value();
        if (order == 0) {
            add_aux_var_names(fid, elem_var_names);
            this->elem_aux_var_fids.push_back(fid);
        }
        else {
            add_aux_var_names(fid, nodal_var_names);
            this->nodal_aux_var_fids.push_back(fid);
        }
    }
    this->exo->write_nodal_var_names(nodal_var_names);
    this->exo->write_elem_var_names(elem_var_names);
    this->exo->write_global_var_names(this->global_var_names);
}

void
ExodusIIOutput::write_variables()
{
    CALL_STACK_MSG();
    Real time = get_problem()->get_time();
    this->exo->write_time(this->step_num, time);

    write_field_variables();
    write_global_variables();

    this->exo->update();
}

void
ExodusIIOutput::write_field_variables()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();
    dpi->compute_solution_vector_local();
    if (this->cont)
        write_nodal_variables_continuous();
    else if (this->discont)
        write_nodal_variables_discontinuous();
    write_elem_variables();
}

void
ExodusIIOutput::write_nodal_variables_continuous()
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();

    auto sln = dpi->get_solution_vector_local();
    const Scalar * sln_vals = sln ? sln.get_array_read() : nullptr;

    auto aux_sln = dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;

    Int n_all_elems = this->mesh->get_num_all_cells();

    for (auto n : this->mesh->get_vertex_range()) {
        int exo_var_id = 1;
        if (sln_vals) {
            for (auto fid : this->nodal_var_fids) {
                auto offset = dpi->get_field_dof(n, fid);
                auto nc = dpi->get_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    int exo_idx = (int) (n - n_all_elems + 1);
                    this->exo->write_partial_nodal_var(this->step_num,
                                                       exo_var_id,
                                                       1,
                                                       exo_idx,
                                                       sln_vals[offset + c]);
                }
            }
        }
        if (aux_sln_vals) {
            for (auto fid : this->nodal_aux_var_fids) {
                auto offset = dpi->get_aux_field_dof(n, fid);
                auto nc = dpi->get_aux_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    int exo_idx = (int) (n - n_all_elems + 1);
                    this->exo->write_partial_nodal_var(this->step_num,
                                                       exo_var_id,
                                                       1,
                                                       exo_idx,
                                                       aux_sln_vals[offset + c]);
                }
            }
        }
    }

    if (aux_sln)
        aux_sln.restore_array_read(aux_sln_vals);
    if (sln)
        sln.restore_array_read(sln_vals);
}

void
ExodusIIOutput::write_nodal_variables_discontinuous()
{
    CALL_STACK_MSG();
    auto dgpi = this->dgpi.value();
    auto sln = dgpi->get_solution_vector_local();
    const Scalar * sln_vals = sln ? sln.get_array_read() : nullptr;

    auto aux_sln = dgpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;

    int n_nodes_per_elem = get_num_nodes_per_element();
    for (auto & cid : this->mesh->get_cell_range()) {
        int exo_var_id = 1;
        if (sln_vals) {
            for (auto fid : this->nodal_var_fids) {
                Int nc = dgpi->get_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    for (Int lni = 0; lni < n_nodes_per_elem; ++lni) {
                        Int exo_idx = (cid * n_nodes_per_elem + lni) + 1;
                        Int offset = dgpi->get_field_dof(cid, lni, fid);
                        this->exo->write_partial_nodal_var(this->step_num,
                                                           exo_var_id,
                                                           1,
                                                           exo_idx,
                                                           sln_vals[offset + c]);
                    }
                }
            }
        }
        if (aux_sln_vals) {
            for (auto fid : this->nodal_aux_var_fids) {
                Int nc = dgpi->get_aux_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    for (Int lni = 0; lni < n_nodes_per_elem; ++lni) {
                        Int exo_idx = (cid * n_nodes_per_elem + lni) + 1;
                        Int offset = dgpi->get_aux_field_dof(cid, lni, fid);
                        this->exo->write_partial_nodal_var(this->step_num,
                                                           exo_var_id,
                                                           1,
                                                           exo_idx,
                                                           aux_sln_vals[offset + c]);
                    }
                }
            }
        }
    }

    if (aux_sln)
        aux_sln.restore_array_read(aux_sln_vals);
    if (sln)
        sln.restore_array_read(sln_vals);
}

void
ExodusIIOutput::write_elem_variables()
{
    CALL_STACK_MSG();
    Int n_cells_sets = this->mesh->get_num_cell_sets();
    if (n_cells_sets > 1) {
        auto cell_sets_label = this->mesh->get_label("Cell Sets");
        auto cell_set_is = cell_sets_label.get_value_index_set();
        auto cell_set_idx = cell_set_is.borrow_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cell_stratum = cell_sets_label.get_stratum(cell_set_idx[i]);
            auto cells = cell_stratum.borrow_indices();
            write_block_elem_variables((int) cell_set_idx[i], cells.size(), cells.data());
        }
    }
    else
        write_block_elem_variables(SINGLE_BLK_ID);
}

void
ExodusIIOutput::write_block_elem_variables(int blk_id, Int n_elems_in_block, const Int * cells)
{
    CALL_STACK_MSG();
    auto dpi = get_discrete_problem_interface();

    Range elem_range;
    if (cells == nullptr) {
        elem_range = this->mesh->get_cell_range();
        n_elems_in_block = elem_range.size();
    }

    auto sln = dpi->get_solution_vector_local();
    const Scalar * sln_vals = sln ? sln.get_array_read() : nullptr;

    auto aux_sln = dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;

    for (Int i = 0; i < n_elems_in_block; ++i) {
        Int elem_id;
        if (cells == nullptr)
            elem_id = elem_range.first() + i;
        else
            elem_id = cells[i];

        int exo_var_id = 1;
        if (sln_vals) {
            for (auto & fid : this->elem_var_fids) {
                auto offset = dpi->get_field_dof(elem_id, fid);
                auto nc = dpi->get_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    int exo_idx = (int) (i + 1);
                    this->exo->write_partial_elem_var(this->step_num,
                                                      exo_var_id,
                                                      blk_id,
                                                      exo_idx,
                                                      sln_vals[offset + c]);
                }
            }
        }
        if (aux_sln_vals) {
            for (auto & fid : this->elem_aux_var_fids) {
                auto offset = dpi->get_aux_field_dof(elem_id, fid);
                auto nc = dpi->get_aux_field_num_components(fid).value();
                for (Int c = 0; c < nc; ++c, ++exo_var_id) {
                    int exo_idx = (int) (i + 1);
                    this->exo->write_partial_elem_var(this->step_num,
                                                      exo_var_id,
                                                      blk_id,
                                                      exo_idx,
                                                      aux_sln_vals[offset + c]);
                }
            }
        }
    }

    if (aux_sln)
        aux_sln.restore_array_read(aux_sln_vals);
    if (sln)
        sln.restore_array_read(sln_vals);
}

void
ExodusIIOutput::write_global_variables()
{
    CALL_STACK_MSG();

    int exo_var_id = 1;
    for (auto & name : this->global_var_names) {
        auto pp = get_problem()->get_postprocessor(name).value();
        auto vals = pp->get_value();
        // FIXME: store all components
        this->exo->write_global_var(this->step_num, exo_var_id, vals[0]);
        ++exo_var_id;
    }
}

void
ExodusIIOutput::write_info()
{
    CALL_STACK_MSG();
    auto app = get_app();
    std::time_t now = std::time(nullptr);
    String datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));
    String created_by =
        fmt::format("Created by {} {}, on {}", app->get_name(), app->get_version(), datetime);

    std::vector<std::string> info;
    info.push_back(created_by);
    this->exo->write_info(info);
}

void
ExodusIIOutput::write_block_connectivity_continuous(int blk_id,
                                                    PolytopeType polytope_type,
                                                    Int n_elems_in_block,
                                                    const Int * cells)
{
    CALL_STACK_MSG();
    auto dm = this->mesh->get_dm();
    Int n_all_elems = this->mesh->get_num_all_cells();
    const char * elem_type = get_elem_type(polytope_type);
    int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);
    const Int * ordering = get_elem_node_ordering(polytope_type);
    std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
    for (Int i = 0, j = 0; i < n_elems_in_block; ++i) {
        Int elem_id = cells[i];
        Int closure_size;
        Int * closure = nullptr;
        PETSC_CHECK(DMPlexGetTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
        for (Int k = 0; k < n_nodes_per_elem; ++k, ++j) {
            Int l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
            connect[j] = (int) (closure[l] - n_all_elems + 1);
        }
        PETSC_CHECK(
            DMPlexRestoreTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
    }
    this->exo->write_block(blk_id, elem_type, n_elems_in_block, connect);
}

void
ExodusIIOutput::write_block_connectivity_discontinuous(int blk_id,
                                                       PolytopeType polytope_type,
                                                       Int n_elems_in_block,
                                                       const Int * cells)
{
    CALL_STACK_MSG();
    const char * elem_type = get_elem_type(polytope_type);
    int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);
    std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
    for (Int i = 0, j = 0; i < n_elems_in_block; ++i) {
        auto cell_id = cells[i];
        for (Int k = 0; k < n_nodes_per_elem; ++k, ++j)
            connect[j] = (int) (cell_id * n_nodes_per_elem + k + 1);
    }
    this->exo->write_block(blk_id, elem_type, n_elems_in_block, connect);
}

int
ExodusIIOutput::get_num_nodes_per_element()
{
    CALL_STACK_MSG();
    int n_nodes_per_elem = 0;
    int n_elems = (int) this->mesh->get_num_cells();
    if (n_elems > 0) {
        auto ct = this->mesh->get_cell_type(0);
        n_nodes_per_elem = this->mesh->get_num_cell_nodes(ct);
    }
    return n_nodes_per_elem;
}

} // namespace godzilla
