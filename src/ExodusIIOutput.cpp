#include "Godzilla.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Postprocessor.h"
#include "IndexSet.h"
#include "exodusIIcpp.h"
#include "fmt/printf.h"
#include "fmt/format.h"
#include "fmt/chrono.h"
#include <cassert>

namespace godzilla {

const int ExodusIIOutput::SINGLE_BLK_ID = 0;

namespace {

const char *
get_elem_type(DMPolytopeType elem_type)
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

int
get_num_elem_nodes(DMPolytopeType elem_type)
{
    _F_;
    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return 2;
    case DM_POLYTOPE_TRIANGLE:
        return 3;
    case DM_POLYTOPE_QUADRILATERAL:
    case DM_POLYTOPE_TETRAHEDRON:
        return 4;
    case DM_POLYTOPE_HEXAHEDRON:
        return 8;
    default:
        error("Unsupported type.");
    }
}

const PetscInt *
get_elem_node_ordering(DMPolytopeType elem_type)
{
    _F_;

    static const PetscInt seg_ordering[] = { 0, 1 };
    static const PetscInt tri_ordering[] = { 0, 1, 2 };
    static const PetscInt quad_ordering[] = { 0, 1, 2, 3 };
    static const PetscInt tet_ordering[] = { 1, 0, 2, 3 };
    static const PetscInt hex_ordering[] = { 0, 3, 2, 1, 4, 5, 6, 7, 8 };

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

const PetscInt *
get_elem_side_ordering(DMPolytopeType elem_type)
{
    static const PetscInt seg_ordering[] = { 1, 2 };
    static const PetscInt tri_ordering[] = { 1, 2, 3 };
    static const PetscInt quad_ordering[] = { 1, 2, 3, 4 };
    static const PetscInt tet_ordering[] = { 4, 1, 2, 3 };
    static const PetscInt hex_ordering[] = { 5, 6, 1, 3, 2, 4 };

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

} // namespace

REGISTER_OBJECT(ExodusIIOutput);

Parameters
ExodusIIOutput::parameters()
{
    Parameters params = FileOutput::parameters();
    params.add_param<std::vector<std::string>>(
        "variables",
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const Parameters & params) :
    FileOutput(params),
    variable_names(get_param<std::vector<std::string>>("variables")),
    dpi(dynamic_cast<const DiscreteProblemInterface *>(this->problem)),
    mesh(this->problem ? dynamic_cast<const UnstructuredMesh *>(this->problem->get_mesh())
                       : nullptr),
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
    if (get_comm_size() == 1)
        this->file_name = fmt::sprintf("%s.%s", this->file_base, this->get_file_ext());
    else
        this->file_name =
            fmt::sprintf("%s.%d.%s", this->file_base, get_processor_id(), this->get_file_ext());
}

void
ExodusIIOutput::create()
{
    _F_;
    FileOutput::create();

    assert(this->dpi != nullptr);
    assert(this->problem != nullptr);

    auto flds = this->dpi->get_field_names();
    auto & pps = this->problem->get_postprocessor_names();

    if (this->variable_names.empty()) {
        this->field_var_names = flds;
        this->global_var_names = pps;
    }
    else {
        std::set<std::string> field_names(flds.begin(), flds.end());
        std::set<std::string> pp_names(pps.begin(), pps.end());

        for (auto & name : this->variable_names) {
            if (field_names.count(name) == 1)
                this->field_var_names.push_back(name);
            else if (pp_names.count(name) == 1)
                this->global_var_names.push_back(name);
            else
                log_error("Variable '%s' specified in 'variables' parameter does not exist. Typo?",
                          name);
        }
    }
}

void
ExodusIIOutput::check()
{
    _F_;
    if (this->dpi == nullptr)
        log_error("ExodusII output can be only used with finite element problems.");
    if (this->mesh == nullptr)
        log_error("ExodusII output can be only used with unstructured meshes.");
}

void
ExodusIIOutput::output_step()
{
    _F_;
    // We only have fixed meshes, so no need to deal with a sequence of files
    set_file_name();
    lprintf(9, "Output to file: %s", this->file_name);

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
        error("Could not open file '%s' for writing.", get_file_name());
}

void
ExodusIIOutput::write_mesh()
{
    _F_;
    int n_nodes = (int) this->mesh->get_num_vertices();
    int n_elems = (int) this->mesh->get_num_elements();

    // number of element blocks
    int n_elem_blk = (int) this->mesh->get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int n_node_sets = (int) this->mesh->get_num_vertex_sets();
    int n_side_sets = (int) this->mesh->get_num_face_sets();

    int exo_dim = (int) this->mesh->get_dimension();
    // Visualization SW based on VTK have problems showing 1D, so we cast it like a 2D problem with
    // y-coordinate equal to zero
    if (exo_dim == 1)
        exo_dim = 2;
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
    Vec coord = this->dpi->get_coordinates_local();
    PetscInt coord_size;
    PETSC_CHECK(VecGetSize(coord, &coord_size));
    PetscScalar * xyz;
    PETSC_CHECK(VecGetArray(coord, &xyz));

    int n_nodes = (int) coord_size / dim;
    std::vector<double> x(n_nodes, 0.);
    std::vector<double> y;
    std::vector<double> z;

    if (exo_dim >= 2)
        y.resize(n_nodes);
    if (exo_dim >= 3)
        z.resize(n_nodes);
    for (PetscInt i = 0; i < n_nodes; i++) {
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

    PETSC_CHECK(VecRestoreArray(coord, &xyz));

    this->exo->write_coord_names();
}

void
ExodusIIOutput::write_elements()
{
    _F_;
    std::vector<std::string> block_names;

    PetscInt n_cells_sets = this->mesh->get_num_cell_sets();

    if (n_cells_sets > 1) {
        block_names.resize(n_cells_sets);

        DMLabel cell_sets_label = this->mesh->get_label("Cell Sets");
        IndexSet cell_set_idx = IndexSet::values_from_label(cell_sets_label);
        for (PetscInt i = 0; i < n_cells_sets; ++i) {
            IndexSet cells = IndexSet::stratum_from_label(cell_sets_label, cell_set_idx[i]);
            write_block_connectivity((int) cell_set_idx[i], cells.size(), cells.data());

            block_names[i] = this->mesh->get_cell_set_name(cell_set_idx[i]);
        }
    }
    else
        write_block_connectivity(SINGLE_BLK_ID);

    this->exo->write_block_names(block_names);
}

void
ExodusIIOutput::write_node_sets()
{
    _F_;
    if (!this->mesh->has_label("Vertex Sets"))
        return;

    PetscInt elem_first, elem_last;
    this->mesh->get_element_idx_range(elem_first, elem_last);
    PetscInt n_elems_in_block = elem_last - elem_first;

    PetscInt n_node_sets = this->mesh->get_num_vertex_sets();
    DMLabel vertex_sets_label = this->mesh->get_label("Vertex Sets");
    IndexSet vertex_set_idx = IndexSet::values_from_label(vertex_sets_label);
    for (PetscInt i = 0; i < n_node_sets; ++i) {
        IndexSet vertices = IndexSet::stratum_from_label(vertex_sets_label, vertex_set_idx[i]);
        PetscInt n_nodes_in_set = vertices.size();
        std::vector<int> node_set(n_nodes_in_set);
        for (PetscInt j = 0; j < n_nodes_in_set; j++)
            node_set[j] = (int) (vertices[j] - n_elems_in_block + 1);
        this->exo->write_node_set(vertex_set_idx[i], node_set);

        // TODO: set the node set name
    }
}

void
ExodusIIOutput::write_face_sets()
{
    _F_;
    if (!this->mesh->has_label("Face Sets"))
        return;

    DM dm = this->mesh->get_dm();
    std::vector<std::string> fs_names;

    DMLabel face_sets_label;
    PETSC_CHECK(DMGetLabel(dm, "Face Sets", &face_sets_label));

    PetscInt n_side_sets = this->mesh->get_num_face_sets();
    fs_names.resize(n_side_sets);

    IndexSet face_set_idx = IndexSet::values_from_label(face_sets_label);
    for (PetscInt fs = 0; fs < n_side_sets; ++fs) {
        IndexSet faces = IndexSet::stratum_from_label(face_sets_label, face_set_idx[fs]);
        PetscInt face_set_size = faces.size();
        std::vector<int> elem_list(face_set_size);
        std::vector<int> side_list(face_set_size);
        for (PetscInt i = 0; i < face_set_size; ++i) {
            // Element
            PetscInt num_points;
            PetscInt * points = nullptr;
            PETSC_CHECK(
                DMPlexGetTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            PetscInt el = points[2];
            DMPolytopeType polytope_type = this->mesh->get_cell_type(el);
            const PetscInt * side_ordering = get_elem_side_ordering(polytope_type);
            elem_list[i] = (int) (el + 1);

            PETSC_CHECK(
                DMPlexRestoreTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            // Side
            points = nullptr;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));

            for (PetscInt j = 1; j < num_points; ++j) {
                if (points[j * 2] == faces[i]) {
                    side_list[i] = (int) side_ordering[j - 1];
                    break;
                }
            }

            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));
        }
        this->exo->write_side_set(face_set_idx[fs], elem_list, side_list);

        fs_names[fs] = this->mesh->get_face_set_name(face_set_idx[fs]);
    }

    this->exo->write_side_set_names(fs_names);
}

void
ExodusIIOutput::add_var_names(PetscInt fid, std::vector<std::string> & var_names)
{
    const std::string & name = this->dpi->get_field_name(fid);
    PetscInt nc = this->dpi->get_field_num_components(fid);
    if (nc == 1)
        var_names.push_back(name);
    else {
        for (PetscInt c = 0; c < nc; c++) {
            std::string comp_name = this->dpi->get_field_component_name(fid, c);
            std::string s;
            if (comp_name.length() == 0)
                s = fmt::sprintf("%s_%d", name, c);
            else
                s = fmt::sprintf("%s_%s", name, comp_name);
            var_names.push_back(s);
        }
    }
}

void
ExodusIIOutput::write_all_variable_names()
{
    _F_;

    this->nodal_var_fids.clear();
    this->elem_var_fids.clear();
    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names) {
        PetscInt fid = this->dpi->get_field_id(name);
        PetscInt order = this->dpi->get_field_order(fid);
        if (order == 0) {
            add_var_names(fid, elem_var_names);
            this->elem_var_fids.push_back(fid);
        }
        else {
            add_var_names(fid, nodal_var_names);
            this->nodal_var_fids.push_back(fid);
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
    PetscReal time = this->problem->get_time();
    this->exo->write_time(this->step_num, time);

    write_field_variables();
    write_global_variables();

    this->exo->update();
}

void
ExodusIIOutput::write_field_variables()
{
    _F_;
    Vec sln = this->dpi->get_solution_vector_local();

    const PetscScalar * sln_vals;
    PETSC_CHECK(VecGetArrayRead(sln, &sln_vals));

    write_nodal_variables(sln_vals);
    write_elem_variables(sln_vals);

    PETSC_CHECK(VecRestoreArrayRead(sln, &sln_vals));
}

void
ExodusIIOutput::write_nodal_variables(const PetscScalar * sln)
{
    _F_;

    PetscInt n_all_elems = this->mesh->get_num_all_elements();
    PetscInt first, last;
    this->mesh->get_vertex_idx_range(first, last);

    for (PetscInt n = first; n < last; n++) {
        int exo_var_id = 1;
        for (auto fid : this->nodal_var_fids) {
            PetscInt offset = this->dpi->get_field_dof(n, fid);
            PetscInt nc = this->dpi->get_field_num_components(fid);
            for (PetscInt c = 0; c < nc; c++, exo_var_id++) {
                int exo_idx = (int) (n - n_all_elems + 1);
                this->exo->write_partial_nodal_var(this->step_num,
                                                   exo_var_id,
                                                   1,
                                                   exo_idx,
                                                   sln[offset + c]);
            }
        }
    }
}

void
ExodusIIOutput::write_elem_variables(const PetscScalar * sln)
{
    _F_;
    PetscInt n_cells_sets = this->mesh->get_num_cell_sets();
    if (n_cells_sets > 1) {
        // TODO: go block by block and save the elemental variables
        error("Block-restricted elemental variable output is not implemented, yet.");
    }
    else
        write_block_elem_variables(SINGLE_BLK_ID, sln);
}

void
ExodusIIOutput::write_block_elem_variables(int blk_id, const PetscScalar * sln)
{
    _F_;
    PetscInt first, last;
    this->mesh->get_element_idx_range(first, last);

    for (PetscInt n = first; n < last; n++) {
        int exo_var_id = 1;
        for (auto & fid : this->elem_var_fids) {
            PetscInt offset = this->dpi->get_field_dof(n, fid);
            PetscInt nc = this->dpi->get_field_num_components(fid);
            for (PetscInt c = 0; c < nc; c++, exo_var_id++) {
                int exo_idx = (int) (n - first + 1);
                this->exo->write_partial_elem_var(this->step_num,
                                                  exo_var_id,
                                                  blk_id,
                                                  exo_idx,
                                                  sln[offset + c]);
            }
        }
    }
}

void
ExodusIIOutput::write_global_variables()
{
    _F_;

    int exo_var_id = 1;
    for (auto & name : this->global_var_names) {
        Postprocessor * pp = this->problem->get_postprocessor(name);
        PetscReal val = pp->get_value();
        this->exo->write_global_var(this->step_num, exo_var_id, val);
        exo_var_id++;
    }
}

void
ExodusIIOutput::write_info()
{
    _F_;
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", fmt::localtime(now));
    std::string created_by =
        fmt::format("Created by godzilla {}, on {}", GODZILLA_VERSION, datetime);

    std::vector<std::string> info;
    info.push_back(created_by);
    this->exo->write_info(info);
}

void
ExodusIIOutput::write_block_connectivity(int blk_id,
                                         PetscInt n_elems_in_block,
                                         const PetscInt * cells)
{
    _F_;
    DM dm = this->mesh->get_dm();
    PetscInt n_all_elems = this->mesh->get_num_all_elements();
    PetscInt elem_first, elem_last;
    DMPolytopeType polytope_type;

    if (cells == nullptr) {
        this->mesh->get_element_idx_range(elem_first, elem_last);
        n_elems_in_block = this->mesh->get_num_elements();
        polytope_type = this->mesh->get_cell_type(elem_first);
    }
    else
        polytope_type = this->mesh->get_cell_type(cells[0]);

    const char * elem_type = get_elem_type(polytope_type);
    int n_nodes_per_elem = get_num_elem_nodes(polytope_type);
    const PetscInt * ordering = get_elem_node_ordering(polytope_type);
    std::vector<int> connect((std::size_t) n_elems_in_block * n_nodes_per_elem);
    for (PetscInt i = 0, j = 0; i < n_elems_in_block; i++) {
        PetscInt elem_id;
        if (cells == nullptr)
            elem_id = elem_first + i;
        else
            elem_id = cells[i];
        PetscInt closure_size;
        PetscInt * closure = nullptr;
        PETSC_CHECK(DMPlexGetTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
        for (PetscInt k = 0; k < n_nodes_per_elem; k++, j++) {
            PetscInt l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
            connect[j] = (int) (closure[l] - n_all_elems + 1);
        }
        PETSC_CHECK(
            DMPlexRestoreTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
    }
    this->exo->write_block(blk_id, elem_type, n_elems_in_block, connect);
}

} // namespace godzilla
