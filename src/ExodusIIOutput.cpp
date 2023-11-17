#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "godzilla/ExodusIIOutput.h"
#include "godzilla/Problem.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/IndexSet.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include "fmt/printf.h"
#include "fmt/format.h"
#include "fmt/chrono.h"
#include <set>
#include <cassert>

namespace godzilla {

const int ExodusIIOutput::SINGLE_BLK_ID = 0;

const char *
ExodusIIOutput::get_elem_type(DMPolytopeType elem_type)
{
    _F_;
    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return "BAR2";
    case DM_POLYTOPE_TRIANGLE:
        return "TRI3";
    case DM_POLYTOPE_QUADRILATERAL:
        return "QUAD4";
    case DM_POLYTOPE_TETRAHEDRON:
        return "TET4";
    case DM_POLYTOPE_HEXAHEDRON:
        return "HEX8";
    default:
        error("Unsupported type.");
    }
}

const Int *
ExodusIIOutput::get_elem_node_ordering(DMPolytopeType elem_type)
{
    _F_;
    static const Int seg_ordering[] = { 0, 1 };
    static const Int tri_ordering[] = { 0, 1, 2 };
    static const Int quad_ordering[] = { 0, 1, 2, 3 };
    static const Int tet_ordering[] = { 1, 0, 2, 3 };
    static const Int hex_ordering[] = { 0, 3, 2, 1, 4, 5, 6, 7, 8 };

    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return seg_ordering;
    case DM_POLYTOPE_TRIANGLE:
        return tri_ordering;
    case DM_POLYTOPE_QUADRILATERAL:
        return quad_ordering;
    case DM_POLYTOPE_TETRAHEDRON:
        return tet_ordering;
    case DM_POLYTOPE_HEXAHEDRON:
        return hex_ordering;
    default:
        error("Unsupported type.");
    }
}

const Int *
ExodusIIOutput::get_elem_side_ordering(DMPolytopeType elem_type)
{
    _F_;
    static const Int seg_ordering[] = { 1, 2 };
    static const Int tri_ordering[] = { 1, 2, 3 };
    static const Int quad_ordering[] = { 1, 2, 3, 4 };
    static const Int tet_ordering[] = { 4, 1, 2, 3 };
    static const Int hex_ordering[] = { 5, 6, 1, 3, 2, 4 };

    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return seg_ordering;
    case DM_POLYTOPE_TRIANGLE:
        return tri_ordering;
    case DM_POLYTOPE_QUADRILATERAL:
        return quad_ordering;
    case DM_POLYTOPE_TETRAHEDRON:
        return tet_ordering;
    case DM_POLYTOPE_HEXAHEDRON:
        return hex_ordering;
    default:
        error("Unsupported type.");
    }
}

REGISTER_OBJECT(ExodusIIOutput);

Parameters
ExodusIIOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    params.add_private_param<UnstructuredMesh *>("_mesh", nullptr);
    params.add_param<std::vector<std::string>>(
        "variables",
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const Parameters & params) :
    FileOutput(params),
    variable_names(get_param<std::vector<std::string>>("variables")),
    dpi(dynamic_cast<DiscreteProblemInterface *>(this->problem)),
    mesh(this->problem ? dynamic_cast<UnstructuredMesh *>(this->problem->get_mesh())
                       : get_param<UnstructuredMesh *>("_mesh")),
    exo(nullptr),
    step_num(1),
    mesh_stored(false)
{
    _F_;
}

ExodusIIOutput::~ExodusIIOutput()
{
    _F_;
    if (this->exo)
        this->exo->close();
}

std::string
ExodusIIOutput::get_file_ext() const
{
    _F_;
    return { "exo" };
}

void
ExodusIIOutput::set_file_name()
{
    _F_;
    if (get_comm().size() == 1)
        this->file_name = fmt::format("{}.{}", this->file_base, this->get_file_ext());
    else
        this->file_name =
            fmt::format("{}.{}.{}", this->file_base, get_processor_id(), this->get_file_ext());
}

void
ExodusIIOutput::create()
{
    _F_;
    FileOutput::create();

    if (this->dpi) {
        auto flds = this->dpi->get_field_names();
        auto aux_flds = this->dpi->get_aux_field_names();
        auto & pps = this->problem->get_postprocessor_names();

        if (this->variable_names.empty()) {
            this->field_var_names = flds;
            this->aux_field_var_names = aux_flds;
            this->global_var_names = pps;
        }
        else {
            std::set<std::string> field_names(flds.begin(), flds.end());
            std::set<std::string> aux_field_names(aux_flds.begin(), aux_flds.end());
            std::set<std::string> pp_names(pps.begin(), pps.end());

            for (auto & name : this->variable_names) {
                if (field_names.count(name) == 1)
                    this->field_var_names.push_back(name);
                else if (aux_field_names.count(name) == 1)
                    this->aux_field_var_names.push_back(name);
                else if (pp_names.count(name) == 1)
                    this->global_var_names.push_back(name);
                else
                    log_error(
                        "Variable '{}' specified in 'variables' parameter does not exist. Typo?",
                        name);
            }
        }
    }
}

void
ExodusIIOutput::check()
{
    _F_;
    FileOutput::check();
    if (this->mesh == nullptr)
        log_error("ExodusII output can be only used with unstructured meshes.");
}

void
ExodusIIOutput::output_mesh()
{
    _F_;
    // We only have fixed meshes, so no need to deal with a sequence of files
    set_file_name();
    TIMED_EVENT(9, "ExodusIIOutput", "Output to file: {}", this->file_name);

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
    _F_;
    // We only have fixed meshes, so no need to deal with a sequence of files
    set_file_name();
    TIMED_EVENT(9, "ExodusIIOutput", "Output to file: {}", this->file_name);

    if (this->exo == nullptr)
        open_file();

    if (!this->mesh_stored) {
        write_info();
        write_mesh();
        write_all_variable_names();
    }

    write_variables();
    this->step_num++;
}

void
ExodusIIOutput::open_file()
{
    _F_;
    this->exo = new exodusIIcpp::File(this->file_name, exodusIIcpp::FileAccess::WRITE);
    if (!this->exo->is_opened())
        error("Could not open file '{}' for writing.", get_file_name());
}

void
ExodusIIOutput::write_mesh()
{
    _F_;
    int n_nodes = (int) this->mesh->get_num_vertices();
    int n_elems = (int) this->mesh->get_num_cells();

    // number of element blocks
    int n_elem_blk = (int) this->mesh->get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int n_node_sets = (int) this->mesh->get_num_vertex_sets();
    int n_side_sets = (int) this->mesh->get_num_face_sets();

    int exo_dim = (int) this->mesh->get_dimension();
    this->exo->init("", exo_dim, n_nodes, n_elems, n_elem_blk, n_node_sets, n_side_sets);

    write_coords(exo_dim);
    write_elements();
    write_node_sets();
    write_face_sets();

    this->mesh_stored = true;
}

void
ExodusIIOutput::write_coords(int exo_dim)
{
    _F_;
    int dim = (int) this->mesh->get_dimension();
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
    for (Int i = 0; i < n_nodes; i++) {
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
ExodusIIOutput::write_elements()
{
    _F_;
    std::vector<std::string> block_names;

    Int n_cells_sets = this->mesh->get_num_cell_sets();

    if (n_cells_sets > 1) {
        block_names.resize(n_cells_sets);

        auto cell_sets_label = this->mesh->get_label("Cell Sets");
        auto cell_set_idx = cell_sets_label.get_value_index_set();
        cell_set_idx.get_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cells = cell_sets_label.get_stratum(cell_set_idx[i]);
            cells.get_indices();
            auto polytope_type = this->mesh->get_cell_type(cells[0]);
            write_block_connectivity((int) cell_set_idx[i],
                                     polytope_type,
                                     cells.get_size(),
                                     cells.data());

            block_names[i] = this->mesh->get_cell_set_name(cell_set_idx[i]);
            cells.restore_indices();
            cells.destroy();
        }
        cell_set_idx.restore_indices();
        cell_set_idx.destroy();
    }
    else {
        auto depth_label = this->mesh->get_depth_label();
        Int dim = this->mesh->get_dimension();
        IndexSet cells = depth_label.get_stratum(dim);
        cells.get_indices();
        DMPolytopeType polytope_type = this->mesh->get_cell_type(cells[0]);
        auto n_elems = this->mesh->get_num_cells();
        write_block_connectivity(SINGLE_BLK_ID, polytope_type, n_elems, cells.data());
        cells.restore_indices();
        cells.destroy();
    }

    this->exo->write_block_names(block_names);
}

void
ExodusIIOutput::write_node_sets()
{
    _F_;
    if (!this->mesh->has_label("Vertex Sets"))
        return;

    auto elem_range = this->mesh->get_cell_range();
    Int n_elems_in_block = elem_range.size();

    Int n_node_sets = this->mesh->get_num_vertex_sets();
    auto vertex_sets_label = this->mesh->get_label("Vertex Sets");
    auto vertex_set_idx = vertex_sets_label.get_value_index_set();
    vertex_set_idx.get_indices();
    for (Int i = 0; i < n_node_sets; ++i) {
        auto vertices = vertex_sets_label.get_stratum(vertex_set_idx[i]);
        vertices.get_indices();
        Int n_nodes_in_set = vertices.get_size();
        std::vector<int> node_set(n_nodes_in_set);
        for (Int j = 0; j < n_nodes_in_set; j++)
            node_set[j] = (int) (vertices[j] - n_elems_in_block + 1);
        this->exo->write_node_set(vertex_set_idx[i], node_set);

        // TODO: set the node set name

        vertices.restore_indices();
        vertices.destroy();
    }
    vertex_set_idx.restore_indices();
    vertex_set_idx.destroy();
}

void
ExodusIIOutput::write_face_sets()
{
    _F_;
    if (!this->mesh->has_label("Face Sets"))
        return;

    auto dm = this->mesh->dm();
    std::vector<std::string> fs_names;

    auto face_sets_label = this->mesh->get_label("Face Sets");

    Int n_side_sets = this->mesh->get_num_face_sets();
    fs_names.resize(n_side_sets);

    auto face_set_idx = face_sets_label.get_value_index_set();
    face_set_idx.get_indices();
    for (Int fs = 0; fs < n_side_sets; ++fs) {
        auto faces = face_sets_label.get_stratum(face_set_idx[fs]);
        faces.get_indices();
        Int face_set_size = faces.get_size();
        std::vector<int> elem_list(face_set_size);
        std::vector<int> side_list(face_set_size);
        for (Int i = 0; i < face_set_size; ++i) {
            // Element
            Int num_points;
            Int * points = nullptr;
            PETSC_CHECK(
                DMPlexGetTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            Int el = points[2];
            DMPolytopeType polytope_type = this->mesh->get_cell_type(el);
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

        fs_names[fs] = this->mesh->get_face_set_name(face_set_idx[fs]);
        faces.restore_indices();
        faces.destroy();
    }
    face_set_idx.restore_indices();
    face_set_idx.destroy();

    this->exo->write_side_set_names(fs_names);
}

void
ExodusIIOutput::add_var_names(Int fid, std::vector<std::string> & var_names)
{
    _F_;
    const std::string & name = this->dpi->get_field_name(fid);
    Int nc = this->dpi->get_field_num_components(fid);
    if (nc == 1)
        var_names.push_back(name);
    else {
        for (Int c = 0; c < nc; c++) {
            std::string comp_name = this->dpi->get_field_component_name(fid, c);
            std::string s;
            if (comp_name.length() == 0)
                s = fmt::format("{}_{}", name, c);
            else
                s = fmt::format("{}", comp_name);
            var_names.push_back(s);
        }
    }
}

void
ExodusIIOutput::add_aux_var_names(Int fid, std::vector<std::string> & var_names)
{
    _F_;
    const std::string & name = this->dpi->get_aux_field_name(fid);
    Int nc = this->dpi->get_aux_field_num_components(fid);
    if (nc == 1)
        var_names.push_back(name);
    else {
        for (Int c = 0; c < nc; c++) {
            std::string comp_name = this->dpi->get_aux_field_component_name(fid, c);
            std::string s;
            if (comp_name.length() == 0)
                s = fmt::format("{}_{}", name, c);
            else
                s = fmt::format("{}", comp_name);
            var_names.push_back(s);
        }
    }
}

void
ExodusIIOutput::write_all_variable_names()
{
    _F_;

    this->nodal_var_fids.clear();
    this->nodal_aux_var_fids.clear();
    this->elem_var_fids.clear();
    this->elem_aux_var_fids.clear();
    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names) {
        Int fid = this->dpi->get_field_id(name);
        Int order = this->dpi->get_field_order(fid);
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
        Int fid = this->dpi->get_aux_field_id(name);
        Int order = this->dpi->get_aux_field_order(fid);
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
    _F_;
    Real time = this->problem->get_time();
    this->exo->write_time(this->step_num, time);

    write_field_variables();
    write_global_variables();

    this->exo->update();
}

void
ExodusIIOutput::write_field_variables()
{
    _F_;
    write_nodal_variables();
    write_elem_variables();
}

void
ExodusIIOutput::write_nodal_variables()
{
    _F_;

    auto sln = this->dpi->get_solution_vector_local();
    const Scalar * sln_vals = sln.get_array_read();

    auto aux_sln = this->dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;

    Int n_all_elems = this->mesh->get_num_all_cells();

    for (auto n : this->mesh->get_vertex_range()) {
        int exo_var_id = 1;
        for (auto fid : this->nodal_var_fids) {
            Int offset = this->dpi->get_field_dof(n, fid);
            Int nc = this->dpi->get_field_num_components(fid);
            for (Int c = 0; c < nc; c++, exo_var_id++) {
                int exo_idx = (int) (n - n_all_elems + 1);
                this->exo->write_partial_nodal_var(this->step_num,
                                                   exo_var_id,
                                                   1,
                                                   exo_idx,
                                                   sln_vals[offset + c]);
            }
        }
        if (aux_sln_vals) {
            for (auto fid : this->nodal_aux_var_fids) {
                Int offset = this->dpi->get_aux_field_dof(n, fid);
                Int nc = this->dpi->get_aux_field_num_components(fid);
                for (Int c = 0; c < nc; c++, exo_var_id++) {
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
    sln.restore_array_read(sln_vals);
}

void
ExodusIIOutput::write_elem_variables()
{
    _F_;
    Int n_cells_sets = this->mesh->get_num_cell_sets();
    if (n_cells_sets > 1) {
        auto cell_sets_label = this->mesh->get_label("Cell Sets");
        auto cell_set_idx = cell_sets_label.get_value_index_set();
        cell_set_idx.get_indices();
        for (Int i = 0; i < n_cells_sets; ++i) {
            auto cells = cell_sets_label.get_stratum(cell_set_idx[i]);
            write_block_elem_variables((int) cell_set_idx[i], cells.get_size(), cells.data());
            cells.destroy();
        }
        cell_set_idx.restore_indices();
        cell_set_idx.destroy();
    }
    else
        write_block_elem_variables(SINGLE_BLK_ID);
}

void
ExodusIIOutput::write_block_elem_variables(int blk_id, Int n_elems_in_block, const Int * cells)
{
    _F_;
    UnstructuredMesh::Range elem_range;
    if (cells == nullptr) {
        elem_range = this->mesh->get_cell_range();
        n_elems_in_block = elem_range.size();
    }

    auto sln = this->dpi->get_solution_vector_local();
    const Scalar * sln_vals = sln.get_array_read();

    auto aux_sln = this->dpi->get_aux_solution_vector_local();
    const Scalar * aux_sln_vals = (Vec) aux_sln != nullptr ? aux_sln.get_array_read() : nullptr;

    for (Int i = 0; i < n_elems_in_block; i++) {
        Int elem_id;
        if (cells == nullptr)
            elem_id = elem_range.first() + i;
        else
            elem_id = cells[i];

        int exo_var_id = 1;
        for (auto & fid : this->elem_var_fids) {
            Int offset = this->dpi->get_field_dof(elem_id, fid);
            Int nc = this->dpi->get_field_num_components(fid);
            for (Int c = 0; c < nc; c++, exo_var_id++) {
                int exo_idx = (int) (i + 1);
                this->exo->write_partial_elem_var(this->step_num,
                                                  exo_var_id,
                                                  blk_id,
                                                  exo_idx,
                                                  sln_vals[offset + c]);
            }
        }
        if (aux_sln_vals) {
            for (auto & fid : this->elem_aux_var_fids) {
                Int offset = this->dpi->get_aux_field_dof(elem_id, fid);
                Int nc = this->dpi->get_aux_field_num_components(fid);
                for (Int c = 0; c < nc; c++, exo_var_id++) {
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
    sln.restore_array_read(sln_vals);
}

void
ExodusIIOutput::write_global_variables()
{
    _F_;

    int exo_var_id = 1;
    for (auto & name : this->global_var_names) {
        Postprocessor * pp = this->problem->get_postprocessor(name);
        Real val = pp->get_value();
        this->exo->write_global_var(this->step_num, exo_var_id, val);
        exo_var_id++;
    }
}

void
ExodusIIOutput::write_info()
{
    _F_;
    auto app = get_app();
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", fmt::localtime(now));
    std::string created_by =
        fmt::format("Created by {} {}, on {}", app->get_name(), app->get_version(), datetime);

    std::vector<std::string> info;
    info.push_back(created_by);
    this->exo->write_info(info);
}

void
ExodusIIOutput::write_block_connectivity(int blk_id,
                                         DMPolytopeType polytope_type,
                                         Int n_elems_in_block,
                                         const Int * cells)
{
    _F_;
    auto dm = this->mesh->dm();
    Int n_all_elems = this->mesh->get_num_all_cells();
    const char * elem_type = get_elem_type(polytope_type);
    int n_nodes_per_elem = UnstructuredMesh::get_num_cell_nodes(polytope_type);
    const Int * ordering = get_elem_node_ordering(polytope_type);
    std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
    for (Int i = 0, j = 0; i < n_elems_in_block; i++) {
        Int elem_id = cells[i];
        Int closure_size;
        Int * closure = nullptr;
        PETSC_CHECK(DMPlexGetTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
        for (Int k = 0; k < n_nodes_per_elem; k++, j++) {
            Int l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
            connect[j] = (int) (closure[l] - n_all_elems + 1);
        }
        PETSC_CHECK(
            DMPlexRestoreTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
    }
    this->exo->write_block(blk_id, elem_type, n_elems_in_block, connect);
}

} // namespace godzilla
