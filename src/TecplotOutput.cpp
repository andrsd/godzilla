// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/TecplotOutput.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Utils.h"
#include "godzilla/Exception.h"
#include "godzilla/Assert.h"
#include <set>
#include <fmt/chrono.h>

namespace godzilla {

namespace {

#ifdef GODZILLA_WITH_TECIOCPP

teciocpp::ZoneType
get_zone_type(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    switch (elem_type) {
    case PolytopeType::SEGMENT:
        return teciocpp::ZoneType::FELINE;
    case PolytopeType::TRIANGLE:
        return teciocpp::ZoneType::FETRIANGLE;
    case PolytopeType::QUADRILATERAL:
        return teciocpp::ZoneType::FEQUADRILATERAL;
    case PolytopeType::TETRAHEDRON:
        return teciocpp::ZoneType::FETETRAHEDRON;
    case PolytopeType::HEXAHEDRON:
        return teciocpp::ZoneType::FEBRICK;
    default:
        throw InternalError("Unsupported type.");
    }
}

const Int *
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

#endif

} // namespace

Parameters
TecplotOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    params.add_param<std::vector<String>>(
        "variables",
        std::vector<String> {},
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

TecplotOutput::TecplotOutput(const Parameters & pars) :
    FileOutput(pars),
#ifdef GODZILLA_WITH_TECIOCPP
    mesh(get_discrete_problem_interface()->get_mesh()),
    file(nullptr),
    n_zones(0),
#endif
    variable_names(pars.get<std::vector<String>>("variables"))
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
#else
    error("Unable to use TecplotOutput, godzilla was not built with teciocpp.");
#endif
}

TecplotOutput::~TecplotOutput()
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    if (this->file)
        this->file->close();
#endif
}

String
TecplotOutput::get_file_ext() const
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    return "szplt";
#else
    return {};
#endif
}

void
TecplotOutput::create()
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    FileOutput::create();

    auto dpi = get_discrete_problem_interface();

    // Get names of all variables that will be stored
    auto flds = dpi->get_field_names();
    auto aux_flds = dpi->get_aux_field_names();
    if (this->variable_names.empty()) {
        this->field_var_names = flds;
        this->aux_field_var_names = aux_flds;
    }
    else {
        std::set<String> field_names(flds.begin(), flds.end());
        std::set<String> aux_field_names(aux_flds.begin(), aux_flds.end());

        for (auto & name : this->variable_names) {
            if (field_names.count(name) == 1)
                this->field_var_names.push_back(name);
            else if (aux_field_names.count(name) == 1)
                this->aux_field_var_names.push_back(name);
            else
                error("Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                      name);
        }
    }

    // Get field IDs for variables and auxiliary variables
    this->nodal_var_fids.clear();
    this->nodal_aux_var_fids.clear();
    for (auto & name : this->field_var_names) {
        auto fid = dpi->get_field_id(name);
        auto order = dpi->get_field_order(fid.value()).value();
        expect_true(order.value() > 0, "Elemental fields are not supported yet");
        this->nodal_var_fids.push_back(fid.value());
    }
    for (auto & name : this->aux_field_var_names) {
        auto fid = dpi->get_aux_field_id(name);
        auto order = dpi->get_aux_field_order(fid.value()).value();
        expect_true(order.value() > 0, "Auxiliary elemental fields are not supported yet");
        this->nodal_aux_var_fids.push_back(fid.value());
    }

    auto dim = this->mesh->get_dimension();
    expect_true(dim == 1_D || dim == 2_D || dim == 3_D, "Unsupported dimension {}", dim);
    for (Int i = 0; i < dim; ++i)
        this->shared_vars.push_back(true);
    int32_t var_idx = dim;
    for (auto & fid : this->nodal_var_fids) {
        auto nc = dpi->get_field_num_components(fid).value();
        for (Int i = 0; i < nc; ++i) {
            this->nodal_var_idxs.push_back(++var_idx);
            this->shared_vars.push_back(false);
        }
    }
    for (auto & fid : this->nodal_aux_var_fids) {
        auto nc = dpi->get_aux_field_num_components(fid).value();
        for (Int i = 0; i < nc; ++i) {
            this->nodal_aux_var_idxs.push_back(++var_idx);
            this->shared_vars.push_back(false);
        }
    }
#endif
}

void
TecplotOutput::output_step()
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    if (this->file == nullptr)
        open_file();

    write_zone();
    ++this->n_zones;
#endif
}

void
TecplotOutput::open_file()
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    try {
        this->file = new teciocpp::File(get_comm());

        const std::vector<String> coord_names = { "x", "y", "z" };
        auto dim = this->mesh->get_dimension();
        expect_true(dim == 1_D || dim == 2_D || dim == 3_D, "Unsupported dimension {}", dim);
        std::vector<std::string> var_names;
        for (Int i = 0; i < dim; ++i)
            var_names.push_back(coord_names[i]);
        for (auto & fid : this->nodal_var_fids)
            add_var_names(fid, var_names);
        for (auto & fid : this->nodal_aux_var_fids)
            add_aux_var_names(fid, var_names);
        this->file->create(get_file_name(), "", var_names);
    }
    catch (teciocpp::Exception & e) {
        throw Exception("Could not open file '{}' for writing.", get_file_name());
    }
#endif
}

void
TecplotOutput::write_zone()
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    // FIXME: allow output for cell sets
    auto cell_range = this->mesh->get_cell_range();
    auto n_cells_in_block = this->mesh->get_num_cells();
    auto polytope_type = this->mesh->get_cell_type(cell_range.first());
    auto n_nodes = this->mesh->get_num_vertices();

    auto time = get_problem()->get_time();

    if (this->n_zones == 0) {
        auto zn = this->file->zone_create_fe("",
                                             get_zone_type(polytope_type),
                                             n_nodes,
                                             n_cells_in_block,
                                             this->value_locations,
                                             false);
        write_created_by(zn);
        this->file->set_unsteady_option(zn, time);
        write_coordinates(zn);
        write_field_variable_values(zn);
        write_connectivity(zn);
    }
    else {
        auto zn = this->file->zone_create_fe("",
                                             get_zone_type(polytope_type),
                                             n_nodes,
                                             n_cells_in_block,
                                             this->value_locations,
                                             true,
                                             this->shared_vars);
        this->file->set_unsteady_option(zn, time);
        write_field_variable_values(zn);
    }

    this->file->flush({ 1 });
#endif
}

void
TecplotOutput::write_created_by(int32_t zone)
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    auto app = get_app();
    auto now = std::time(nullptr);
    auto datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", *std::localtime(&now));
    auto created_by =
        fmt::format("Created by {} {}, on {}", app->get_name(), app->get_version(), datetime);
    this->file->add_aux_data(zone, "created_by", created_by);
#endif
}

void
TecplotOutput::write_coordinates(int32_t zone)
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    auto rank = get_comm().rank();

    auto dim = this->mesh->get_dimension();
    auto coord_vec = this->mesh->get_coordinates_local();
    auto coord = coord_vec.borrow_array_read();
    auto n_coords = coord_vec.get_size() / dim;
    std::vector<double> xyz(n_coords);
    for (Int d = 0; d < dim; ++d) {
        for (Int i = 0; i < n_coords; ++i) {
            xyz[i] = coord[i * dim + d];
        }
        this->file->zone_var_write(zone, d + 1, rank, xyz);
    }
#endif
}

void
TecplotOutput::write_connectivity(int32_t zone)
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    auto rank = get_comm().rank();
    std::vector<int32_t> connectivity;
    connectivity.reserve(this->mesh->get_num_vertices());
    auto n_all_elems = this->mesh->get_num_all_cells();
    for (auto & cell_id : this->mesh->get_cell_range()) {
        auto polytope_type = this->mesh->get_cell_type(cell_id);
        auto * ordering = get_elem_node_ordering(polytope_type);
        auto cell_connect = this->mesh->get_connectivity(cell_id);
        for (std::size_t k = 0; k < cell_connect.size(); ++k)
            connectivity.push_back(cell_connect[ordering[k]] - n_all_elems + 1);
    }
    this->file->zone_node_map_write(zone, rank, connectivity);
#endif
}

void
TecplotOutput::write_field_variable_values(int32_t zone)
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    write_nodal_field_variable_values(zone);
#endif
}

void
TecplotOutput::write_nodal_field_variable_values(int32_t zone)
{
    CALL_STACK_MSG();
#ifdef GODZILLA_WITH_TECIOCPP
    auto rank = get_comm().rank();
    auto dpi = get_discrete_problem_interface();

    auto n_nodes = this->mesh->get_num_vertices();
    auto n_elems = this->mesh->get_num_cells();
    std::vector<double> vals(n_nodes);

    dpi->compute_solution_vector_local();
    auto sln = dpi->get_solution_vector_local();
    auto sln_vals = sln.borrow_array_read();
    for (auto [j, fid] : enumerate(this->nodal_var_fids)) {
        auto nc = dpi->get_field_num_components(fid).value();
        for (Int c = 0; c < nc; ++c, ++j) {
            for (auto n : this->mesh->get_vertex_range()) {
                auto offset = dpi->get_field_dof(n, fid);
                vals[n - n_elems] = sln_vals[offset + c];
            }
            this->file->zone_var_write(zone, this->nodal_var_idxs[j], rank, vals);
        }
    }

    auto aux_sln = dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;
    if (aux_sln_vals) {
        for (auto [j, fid] : enumerate(this->nodal_aux_var_fids)) {
            auto nc = dpi->get_aux_field_num_components(fid).value();
            for (Int c = 0; c < nc; ++c, ++j) {
                for (auto n : this->mesh->get_vertex_range()) {
                    auto offset = dpi->get_aux_field_dof(n, fid);
                    vals[n - n_elems] = sln_vals[offset + c];
                }
                this->file->zone_var_write(zone, this->nodal_aux_var_idxs[j], rank, vals);
            }
        }
        aux_sln.restore_array_read(aux_sln_vals);
    }
#endif
}

} // namespace godzilla
