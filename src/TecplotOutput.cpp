// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/TecplotOutput.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include "godzilla/Exception.h"
#include <cassert>
#include <set>
#include <fmt/chrono.h>

namespace godzilla {

namespace {

const char *
get_zone_type(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    switch (elem_type) {
    case PolytopeType::SEGMENT:
        return "FELINESEG";
    case PolytopeType::TRIANGLE:
        return "FETRIANGLE";
    case PolytopeType::QUADRILATERAL:
        return "FEQUADRILATERAL";
    case PolytopeType::TETRAHEDRON:
        return "FETETRAHEDRON";
    case PolytopeType::HEXAHEDRON:
        return "FEBRICK";
    default:
        error("Unsupported type.");
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
        error("Unsupported type.");
    }
}

} // namespace

Parameters
TecplotOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    params.add_param<std::vector<std::string>>(
        "variables",
        "List of variables to be stored. If not specified, all variables will be stored.");
    params.add_param<std::string>("format", "binary", "Format of the file [binary, ascii]");
    return params;
}

TecplotOutput::TecplotOutput(const Parameters & params) :
    FileOutput(params),
    format(BINARY),
    dpi(dynamic_cast<DiscreteProblemInterface *>(get_problem())),
    mesh(dpi ? dpi->get_mesh() : nullptr),
    variable_names(get_param<std::vector<std::string>>("variables")),
    file(nullptr),
    header_written(false),
    n_shared_vars(-1),
    n_zones(-1),
    element_id_var_index(-1)
{
    CALL_STACK_MSG();
    if (this->dpi == nullptr)
        log_error("Tecplot output can be only used with finite element problems.");
    if (this->mesh == nullptr)
        log_error("Tecplot output can be only used with unstructured meshes.");
}

TecplotOutput::~TecplotOutput()
{
    CALL_STACK_MSG();
    close_file();
}

std::string
TecplotOutput::get_file_ext() const
{
    CALL_STACK_MSG();
    switch (this->format) {
    case BINARY:
        return { "plt" };
    case ASCII:
        return { "dat" };
    default:
        throw InternalError("Unknown tecplot format");
    }
}

void
TecplotOutput::create()
{
    CALL_STACK_MSG();
    FileOutput::create();

    assert(this->dpi != nullptr);
    assert(get_problem() != nullptr);

    auto fmt_str = get_param<std::string>("format");
    if (validation::in(fmt_str, { "binary", "ascii" })) {
        std::string fmt = utils::to_lower(fmt_str);
        if (fmt == "binary") {
            this->format = BINARY;
            log_error(
                "Output to a binary format is not implemented yet. Use 'format: ascii' instead.");
        }
        else if (fmt == "ascii")
            this->format = ASCII;
    }
    else
        log_error("The 'format' parameter can be either 'binary' or 'ascii'.");

    // Get names of all variables that will be stored
    auto flds = this->dpi->get_field_names();
    auto aux_flds = this->dpi->get_aux_field_names();
    if (this->variable_names.empty()) {
        this->field_var_names = flds;
        this->aux_field_var_names = aux_flds;
    }
    else {
        std::set<std::string> field_names(flds.begin(), flds.end());
        std::set<std::string> aux_field_names(aux_flds.begin(), aux_flds.end());

        for (auto & name : this->variable_names) {
            if (field_names.count(name) == 1)
                this->field_var_names.push_back(name);
            else if (aux_field_names.count(name) == 1)
                this->aux_field_var_names.push_back(name);
            else
                log_error("Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                          name);
        }
    }

    // Get field IDs for variables and auxiliary variables
    this->nodal_var_fids.clear();
    this->nodal_aux_var_fids.clear();
    for (auto & name : this->field_var_names) {
        Int fid = this->dpi->get_field_id(name);
        Int order = this->dpi->get_field_order(fid);
        if (order == 0)
            log_error("Elemental fields are not supported yet");
        else
            this->nodal_var_fids.push_back(fid);
    }
    for (auto & name : this->aux_field_var_names) {
        Int fid = this->dpi->get_aux_field_id(name);
        Int order = this->dpi->get_aux_field_order(fid);
        if (order == 0)
            log_error("Auxiliary elemental fields are not supported yet");
        else
            this->nodal_aux_var_fids.push_back(fid);
    }
}

void
TecplotOutput::output_step()
{
    CALL_STACK_MSG();
    if (this->file == nullptr)
        open_file();

    if (!this->header_written) {
        write_header();
        write_dataset_aux_data();
        this->n_zones = 0;
    }

    write_zone();
    ++this->n_zones;
}

void
TecplotOutput::open_file()
{
    CALL_STACK_MSG();
    const char * mode = this->format == BINARY ? "wb" : "w";
    this->file = std::fopen(get_file_name().c_str(), mode);
    if (this->file == nullptr)
        throw Exception("Could not open file '{}' for writing.", get_file_name());
}

void
TecplotOutput::close_file()
{
    CALL_STACK_MSG();
    std::fclose(this->file);
}

void
TecplotOutput::write_header()
{
    CALL_STACK_MSG();
    switch (this->format) {
    case BINARY:
        write_header_binary();
        break;
    case ASCII:
        write_header_ascii();
        break;
    }
}

void
TecplotOutput::write_dataset_aux_data()
{
    CALL_STACK_MSG();
    switch (this->format) {
    case BINARY:
        break;
    case ASCII:
        write_created_by_ascii();
        break;
    }
}

void
TecplotOutput::write_zone()
{
    CALL_STACK_MSG();
    switch (this->format) {
    case BINARY:
        write_zone_binary();
        break;
    case ASCII:
        write_zone_ascii();
        break;
    }
}

//

void
TecplotOutput::write_header_binary()
{
    CALL_STACK_MSG();
}

void
TecplotOutput::write_zone_binary()
{
    CALL_STACK_MSG();
}

//

void
TecplotOutput::write_header_ascii()
{
    CALL_STACK_MSG();
    auto title = fmt::format("TITLE = \"{}\"\n", "data");
    write_line(title);

    write_line("VARIABLES =\n");

    auto dim = this->mesh->get_dimension();
    std::vector<std::string> coord_names = { "x", "y", "z" };

    std::vector<std::string> dts;
    std::vector<std::string> var_names;
    for (Int i = 0; i < dim; ++i) {
        var_names.push_back(coord_names[i]);
        dts.emplace_back("DOUBLE");
    }
    var_names.emplace_back("Node IDs");
    dts.emplace_back("LONGINT");
    var_names.emplace_back("Element IDs");
    dts.emplace_back("LONGINT");
    this->element_id_var_index = (Int) var_names.size();
    this->n_shared_vars = (Int) var_names.size();
    for (auto & vn : this->field_var_names) {
        var_names.push_back(vn);
        dts.emplace_back("DOUBLE");
    }
    for (auto & vn : this->aux_field_var_names) {
        var_names.push_back(vn);
        dts.emplace_back("DOUBLE");
    }

    this->datatypes = fmt::format("{}", fmt::join(dts, " "));

    for (auto & vn : var_names)
        write_line(fmt::format("\"{}\"\n", vn));
}

void
TecplotOutput::write_created_by_ascii()
{
    CALL_STACK_MSG();
    auto app = get_app();
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", fmt::localtime(now));
    std::string created_by =
        fmt::format("Created by {} {}, on {}", app->get_name(), app->get_version(), datetime);
    write_line(fmt::format("DATASETAUXDATA {} = \"{}\"\n", "created_by", created_by));
}

void
TecplotOutput::write_zone_ascii()
{
    CALL_STACK_MSG();
    // FIXME: allow output for cell sets
    auto cell_range = this->mesh->get_cell_range();
    auto n_cells_in_block = this->mesh->get_num_cells();
    auto polytope_type = this->mesh->get_cell_type(cell_range.first());
    const char * zone_type = get_zone_type(polytope_type);
    Int n_nodes = this->mesh->get_num_vertices();

    write_line("ZONE\n");
    write_line(
        fmt::format(" ZONETYPE={}, Nodes={}, Elements={}\n", zone_type, n_nodes, n_cells_in_block));

    Real time = get_problem()->get_time();
    write_line(fmt::format(" STRANDID=2, SOLUTIONTIME={}\n", time));
    write_line(fmt::format(" DATAPACKING=BLOCK\n"));
    write_line(fmt::format(" VARLOCATION=([{}]=CELLCENTERED)\n", this->element_id_var_index));

    write_line(fmt::format(" DT=({})\n", this->datatypes));

    if (this->n_zones > 0) {
        write_line(fmt::format(" VARSHARELIST = ([1-{}]=1)\n", this->n_shared_vars));
        write_line(fmt::format(" CONNECTIVITYSHAREZONE = 1\n"));
    }

    if (this->n_zones == 0) {
        write_coordinates_ascii();
        write_node_ids_ascii();
        write_element_ids_ascii();
        write_field_variable_values_ascii();
        write_connectivity_ascii();
    }
    else
        write_field_variable_values_ascii();
}

void
TecplotOutput::write_coordinates_ascii()
{
    CALL_STACK_MSG();
    auto dim = this->mesh->get_dimension();
    auto coord = this->mesh->get_coordinates_local();
    auto xyz = coord.get_array_read();
    auto n_coords = coord.get_size() / dim;
    for (Int d = 0; d < dim; ++d) {
        for (Int i = 0; i < n_coords; ++i) {
            write_line(fmt::format(" {:f}", xyz[i * dim + d]));
            if ((i + 1) % 10 == 0)
                write_line("\n");
        }
        write_line("\n");
    }
    coord.restore_array_read(xyz);
}

void
TecplotOutput::write_node_ids_ascii()
{
    CALL_STACK_MSG();
    auto n_elems = this->mesh->get_num_cells();
    for (auto & vertex_id : this->mesh->get_vertex_range()) {
        auto node_id = vertex_id - n_elems;
        write_line(fmt::format(" {}", node_id));
        if ((node_id + 1) % 10 == 0)
            write_line("\n");
    }
    write_line("\n");
}

void
TecplotOutput::write_element_ids_ascii()
{
    CALL_STACK_MSG();
    for (auto & elem_id : this->mesh->get_cell_range()) {
        write_line(fmt::format(" {}", elem_id));
        if ((elem_id + 1) % 10 == 0)
            write_line("\n");
    }
    write_line("\n");
}

void
TecplotOutput::write_connectivity_ascii()
{
    CALL_STACK_MSG();
    Int n_all_elems = this->mesh->get_num_all_cells();
    for (auto & cell_id : this->mesh->get_cell_range()) {
        auto polytope_type = this->mesh->get_cell_type(cell_id);
        const Int * ordering = get_elem_node_ordering(polytope_type);
        auto cell_connect = this->mesh->get_connectivity(cell_id);
        for (std::size_t k = 0; k < cell_connect.size(); ++k)
            write_line(fmt::format(" {}", cell_connect[ordering[k]] - n_all_elems + 1));
        write_line("\n");
    }
}

void
TecplotOutput::write_field_variable_values_ascii()
{
    CALL_STACK_MSG();
    write_nodal_field_variable_values_ascii();
}

void
TecplotOutput::write_nodal_field_variable_values_ascii()
{
    CALL_STACK_MSG();
    this->dpi->compute_solution_vector_local();
    auto sln = this->dpi->get_solution_vector_local();
    const Scalar * sln_vals = sln.get_array_read();
    for (auto fid : this->nodal_var_fids) {
        Int nc = this->dpi->get_field_num_components(fid);
        for (Int c = 0; c < nc; ++c) {
            for (auto n : this->mesh->get_vertex_range()) {
                Int offset = this->dpi->get_field_dof(n, fid);
                write_line(fmt::format(" {:f}", sln_vals[offset + c]));
            }
            write_line("\n");
        }
    }
    sln.restore_array_read(sln_vals);

    auto aux_sln = this->dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;
    if (aux_sln_vals) {
        for (auto fid : this->nodal_aux_var_fids) {
            Int nc = this->dpi->get_aux_field_num_components(fid);
            for (Int c = 0; c < nc; ++c) {
                for (auto n : this->mesh->get_vertex_range()) {
                    Int offset = this->dpi->get_aux_field_dof(n, fid);
                    write_line(fmt::format(" {:f}", aux_sln_vals[offset + c]));
                }
                write_line("\n");
            }
        }
        aux_sln.restore_array_read(aux_sln_vals);
    }
}

void
TecplotOutput::write_line(const std::string & line)
{
    CALL_STACK_MSG();
    assert(this->file != nullptr);
    std::fputs(line.c_str(), this->file);
}

} // namespace godzilla
