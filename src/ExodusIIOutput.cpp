#include "Godzilla.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "DiscreteProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Postprocessor.h"
#include "exodusII.h"
#include <assert.h>

namespace godzilla {

static const unsigned int MAX_DATE_TIME = 255;

const int ExodusIIOutput::SINGLE_BLK_ID = 0;

REGISTER_OBJECT(ExodusIIOutput);

static void
exo_write_variable_names(int exoid,
                         ex_entity_type obj_type,
                         const std::vector<std::string> & var_names)
{
    _F_;

    int n_vars = var_names.size();
    if (n_vars == 0)
        return;

    ex_put_variable_param(exoid, obj_type, n_vars);
    const char * names[n_vars];
    for (int i = 0; i < n_vars; i++)
        names[i] = var_names[i].c_str();
    ex_put_variable_names(exoid, obj_type, n_vars, (char **) names);
}

static void
exo_write_side_set_names(int exoid, const std::vector<std::string> & sset_names)
{
    int n_sset = sset_names.size();
    if (n_sset == 0)
        return;

    const char * names[n_sset];
    for (int i = 0; i < n_sset; i++)
        names[i] = sset_names[i].c_str();
    ex_put_names(exoid, EX_SIDE_SET, (char **) names);
}

static void
exo_write_block_names(int exoid, const std::vector<std::string> & block_names)
{
    int n_blocks = block_names.size();
    if (n_blocks == 0)
        return;

    const char * names[n_blocks];
    for (int i = 0; i < n_blocks; i++)
        names[i] = block_names[i].c_str();
    ex_put_names(exoid, EX_ELEM_BLOCK, (char **) names);
}

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
    exoid(-1),
    step_num(1),
    mesh_stored(false)
{
    _F_;
}

ExodusIIOutput::~ExodusIIOutput()
{
    _F_;
    if (this->exoid != -1)
        ex_close(this->exoid);
}

std::string
ExodusIIOutput::get_file_ext() const
{
    _F_;
    return std::string("exo");
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

    if (this->variable_names.size() == 0) {
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

    if (this->exoid == -1)
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
    int cpu_word_size = sizeof(PetscReal);
    int io_word_size = sizeof(PetscReal);
    this->exoid = ex_create(this->file_name.c_str(), EX_CLOBBER, &cpu_word_size, &io_word_size);
    if (exoid == -1)
        error("Could not open file '%s' for writing.", get_file_name());
}

void
ExodusIIOutput::write_mesh()
{
    _F_;
    int n_nodes = this->mesh->get_num_vertices();
    int n_elems = this->mesh->get_num_elements();

    // number of element blocks
    PetscInt n_elem_blk = this->mesh->get_num_cell_sets();
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    PetscInt n_node_sets = this->mesh->get_num_vertex_sets();
    PetscInt n_side_sets = this->mesh->get_num_face_sets();

    int exo_dim = this->mesh->get_dimension();
    // Visualization SW based on VTK have problems showing 1D, so we cast it like a 2D problem with
    // y-coordinate equal to zero
    if (exo_dim == 1)
        exo_dim = 2;
    ex_put_init(this->exoid, "", exo_dim, n_nodes, n_elems, n_elem_blk, n_node_sets, n_side_sets);

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
    DM dm = this->mesh->get_dm();
    PetscInt dim = this->mesh->get_dimension();
    Vec coord;
    PETSC_CHECK(DMGetCoordinatesLocal(dm, &coord));
    PetscInt coord_size;
    PETSC_CHECK(VecGetSize(coord, &coord_size));
    PetscScalar * xyz;
    PETSC_CHECK(VecGetArray(coord, &xyz));

    int n_nodes = coord_size / dim;
    PetscReal * x = new PetscReal[n_nodes];
    MEM_CHECK(x);
    memset(x, 0., sizeof(PetscReal) * n_nodes);
    PetscReal * y = nullptr;
    if (exo_dim >= 2) {
        y = new PetscReal[n_nodes];
        MEM_CHECK(y);
        memset(y, 0., sizeof(PetscReal) * n_nodes);
    }
    PetscReal * z = nullptr;
    if (exo_dim >= 3) {
        z = new PetscReal[n_nodes];
        MEM_CHECK(z);
        memset(z, 0., sizeof(PetscReal) * n_nodes);
    }
    for (PetscInt i = 0; i < n_nodes; i++) {
        x[i] = xyz[i * dim + 0];
        if (dim >= 2)
            y[i] = xyz[i * dim + 1];
        if (dim >= 3)
            z[i] = xyz[i * dim + 2];
    }
    ex_put_coord(this->exoid, x, y, z);

    delete[] x;
    delete[] y;
    delete[] z;

    PETSC_CHECK(VecRestoreArray(coord, &xyz));

    const char * coord_names[3] = { "x", "y", "z" };
    ex_put_coord_names(this->exoid, (char **) coord_names);
}

const char *
ExodusIIOutput::get_elem_type(DMPolytopeType elem_type) const
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
ExodusIIOutput::get_num_elem_nodes(DMPolytopeType elem_type) const
{
    _F_;
    switch (elem_type) {
    case DM_POLYTOPE_SEGMENT:
        return 2;
    case DM_POLYTOPE_TRIANGLE:
        return 3;
    case DM_POLYTOPE_QUADRILATERAL:
        return 4;
    case DM_POLYTOPE_TETRAHEDRON:
        return 4;
    case DM_POLYTOPE_HEXAHEDRON:
        return 8;
    default:
        error("Unsupported type.");
    }
}

const PetscInt *
ExodusIIOutput::get_elem_node_ordering(DMPolytopeType elem_type) const
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
ExodusIIOutput::get_elem_side_ordering(DMPolytopeType elem_type) const
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

void
ExodusIIOutput::write_elements()
{
    _F_;
    DM dm = this->mesh->get_dm();
    std::vector<std::string> block_names;

    PetscInt n_cells_sets = this->mesh->get_num_cell_sets();

    if (n_cells_sets > 1) {
        block_names.resize(n_cells_sets);

        DMLabel cell_sets_label = this->mesh->get_label("Cell Sets");

        IS cell_sets_is;
        PETSC_CHECK(DMLabelGetValueIS(cell_sets_label, &cell_sets_is));
        const PetscInt * cell_set_idx;
        PETSC_CHECK(ISGetIndices(cell_sets_is, &cell_set_idx));

        for (PetscInt i = 0; i < n_cells_sets; ++i) {
            IS stratum_is;
            PETSC_CHECK(DMLabelGetStratumIS(cell_sets_label, cell_set_idx[i], &stratum_is));

            const PetscInt * cells;
            PETSC_CHECK(ISGetIndices(stratum_is, &cells));

            PetscInt n_elems_in_block;
            PETSC_CHECK(ISGetSize(stratum_is, &n_elems_in_block));

            write_block_connectivity(cell_set_idx[i], n_elems_in_block, cells);

            PETSC_CHECK(ISRestoreIndices(stratum_is, &cells));
            PETSC_CHECK(ISDestroy(&stratum_is));

            block_names[i] = this->mesh->get_cell_set_name(cell_set_idx[i]);
        }
        PETSC_CHECK(ISRestoreIndices(cell_sets_is, &cell_set_idx));
        PETSC_CHECK(ISDestroy(&cell_sets_is));
    }
    else
        write_block_connectivity(SINGLE_BLK_ID);

    exo_write_block_names(this->exoid, block_names);
}

void
ExodusIIOutput::write_node_sets()
{
    _F_;
    if (!this->mesh->has_label("Vertex Sets"))
        return;

    DM dm = this->mesh->get_dm();

    PetscInt elem_first, elem_last;
    this->mesh->get_element_idx_range(elem_first, elem_last);
    int n_elems_in_block = elem_last - elem_first;

    PetscInt n_node_sets = this->mesh->get_num_vertex_sets();

    DMLabel vertex_sets_label = this->mesh->get_label("Vertex Sets");

    IS vertex_sets_is;
    PETSC_CHECK(DMLabelGetValueIS(vertex_sets_label, &vertex_sets_is));

    const PetscInt * vertex_set_idx;
    PETSC_CHECK(ISGetIndices(vertex_sets_is, &vertex_set_idx));

    for (PetscInt i = 0; i < n_node_sets; ++i) {
        IS stratum_is;
        PETSC_CHECK(DMLabelGetStratumIS(vertex_sets_label, vertex_set_idx[i], &stratum_is));

        const PetscInt * vertices;
        PETSC_CHECK(ISGetIndices(stratum_is, &vertices));

        PetscInt n_nodes_in_set;
        PETSC_CHECK(ISGetSize(stratum_is, &n_nodes_in_set));

        PetscInt * node_set = new PetscInt[n_nodes_in_set];
        MEM_CHECK(node_set);

        for (PetscInt j = 0; j < n_nodes_in_set; j++)
            node_set[j] = vertices[j] - n_elems_in_block + 1;

        ex_put_set_param(this->exoid, EX_NODE_SET, vertex_set_idx[i], n_nodes_in_set, 0);
        ex_put_set(this->exoid, EX_NODE_SET, vertex_set_idx[i], node_set, nullptr);

        delete[] node_set;

        PETSC_CHECK(ISRestoreIndices(stratum_is, &vertices));

        PETSC_CHECK(ISDestroy(&stratum_is));

        // TODO: set the face name
    }
    PETSC_CHECK(ISRestoreIndices(vertex_sets_is, &vertex_set_idx));

    PETSC_CHECK(ISDestroy(&vertex_sets_is));
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

    IS face_sets_is;
    PETSC_CHECK(DMLabelGetValueIS(face_sets_label, &face_sets_is));

    const PetscInt * face_set_idx;
    PETSC_CHECK(ISGetIndices(face_sets_is, &face_set_idx));

    for (PetscInt fs = 0; fs < n_side_sets; ++fs) {
        IS stratum_is;
        PETSC_CHECK(DMLabelGetStratumIS(face_sets_label, face_set_idx[fs], &stratum_is));

        const PetscInt * faces;
        PETSC_CHECK(ISGetIndices(stratum_is, &faces));

        PetscInt face_set_size;
        PETSC_CHECK(ISGetSize(stratum_is, &face_set_size));

        ex_put_set_param(this->exoid, EX_SIDE_SET, face_set_idx[fs], face_set_size, 0);

        PetscInt * elem_list = new PetscInt[face_set_size];
        MEM_CHECK(elem_list);

        PetscInt * side_list = new PetscInt[face_set_size];
        MEM_CHECK(side_list);

        for (PetscInt i = 0; i < face_set_size; ++i) {
            // Element
            PetscInt num_points;
            PetscInt * points = NULL;
            PETSC_CHECK(
                DMPlexGetTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            PetscInt el = points[2];
            DMPolytopeType polytope_type;
            PETSC_CHECK(DMPlexGetCellType(dm, el, &polytope_type));
            const PetscInt * side_ordering = get_elem_side_ordering(polytope_type);
            elem_list[i] = el + 1;

            PETSC_CHECK(
                DMPlexRestoreTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            // Side
            points = NULL;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));

            for (PetscInt j = 1; j < num_points; ++j) {
                if (points[j * 2] == faces[i]) {
                    side_list[i] = side_ordering[j - 1];
                    break;
                }
            }

            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));
        }

        ex_put_set(this->exoid, EX_SIDE_SET, face_set_idx[fs], elem_list, side_list);

        delete[] side_list;
        delete[] elem_list;

        PETSC_CHECK(ISRestoreIndices(stratum_is, &faces));
        PETSC_CHECK(ISDestroy(&stratum_is));

        fs_names[fs] = this->mesh->get_face_set_name(face_set_idx[fs]);
    }
    PETSC_CHECK(ISRestoreIndices(face_sets_is, &face_set_idx));
    PETSC_CHECK(ISDestroy(&face_sets_is));

    exo_write_side_set_names(this->exoid, fs_names);
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
    exo_write_variable_names(this->exoid, EX_NODAL, nodal_var_names);
    exo_write_variable_names(this->exoid, EX_ELEM_BLOCK, elem_var_names);

    std::vector<std::string> global_var_names;
    for (auto & name : this->global_var_names)
        global_var_names.push_back(name);
    exo_write_variable_names(this->exoid, EX_GLOBAL, global_var_names);
}

void
ExodusIIOutput::write_variables()
{
    _F_;
    PetscReal time = this->problem->get_time();
    ex_put_time(this->exoid, this->step_num, &time);

    write_field_variables();
    write_global_variables();

    ex_update(this->exoid);
}

void
ExodusIIOutput::write_field_variables()
{
    _F_;
    PetscReal time = this->problem->get_time();
    DM dm = this->problem->get_dm();
    Vec sln;
    PETSC_CHECK(DMGetLocalVector(dm, &sln));
    PETSC_CHECK(DMGlobalToLocal(dm, this->problem->get_solution_vector(), INSERT_VALUES, sln));
    PETSC_CHECK(DMPlexInsertBoundaryValues(dm, PETSC_TRUE, sln, time, NULL, NULL, NULL));

    const PetscScalar * sln_vals;
    PETSC_CHECK(VecGetArrayRead(sln, &sln_vals));

    write_nodal_variables(sln_vals);
    write_elem_variables(sln_vals);

    PETSC_CHECK(VecRestoreArrayRead(sln, &sln_vals));

    PETSC_CHECK(DMRestoreLocalVector(dm, &sln));
}

void
ExodusIIOutput::write_nodal_variables(const PetscScalar * sln)
{
    _F_;

    DM dm = this->problem->get_dm();
    PetscInt n_all_elems = this->mesh->get_num_all_elements();
    PetscInt first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(dm, this->mesh->get_dimension(), &first, &last));

    for (PetscInt n = first; n < last; n++) {
        int exo_var_id = 1;
        for (std::size_t i = 0; i < this->nodal_var_fids.size(); i++) {
            PetscInt fid = this->nodal_var_fids[i];

            PetscInt offset = this->dpi->get_field_dof(n, fid);
            PetscInt nc = this->dpi->get_field_num_components(fid);
            for (PetscInt c = 0; c <= nc; c++, exo_var_id++) {
                int exo_idx = n - n_all_elems + 1;
                ex_put_partial_var(this->exoid,
                                   this->step_num,
                                   EX_NODAL,
                                   exo_var_id,
                                   1,
                                   exo_idx,
                                   1,
                                   sln + offset + c);
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
        for (std::size_t i = 0; i < this->elem_var_fids.size(); i++) {
            PetscInt fid = this->elem_var_fids[i];

            PetscInt offset = this->dpi->get_field_dof(n, fid);
            PetscInt nc = this->dpi->get_field_num_components(fid);
            for (PetscInt c = 0; c < nc; c++, exo_var_id++) {
                int exo_idx = n - first + 1;
                ex_put_partial_var(this->exoid,
                                   this->step_num,
                                   EX_ELEM_BLOCK,
                                   exo_var_id,
                                   blk_id,
                                   exo_idx,
                                   1,
                                   sln + offset + c);
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
        ex_put_var(this->exoid, this->step_num, EX_GLOBAL, exo_var_id, 0, 1, &val);
        exo_var_id++;
    }
}

void
ExodusIIOutput::write_info()
{
    _F_;

    time_t rawtime;
    time(&rawtime);
    struct tm * now = localtime(&rawtime);

    char datetime[MAX_DATE_TIME + 1];
    strftime(datetime, MAX_DATE_TIME, "%d %b %Y, %H:%M:%S", now);

    char created_by_info[MAX_LINE_LENGTH];
    snprintf(created_by_info,
             MAX_LINE_LENGTH,
             "Created by godzilla %s, on %s",
             GODZILLA_VERSION,
             datetime);

    char * info[] = { created_by_info };
    int n_info = sizeof(info) / sizeof(char *);
    ex_put_info(this->exoid, n_info, info);
}

void
ExodusIIOutput::write_block_connectivity(int blk_id, int n_elems_in_block, const PetscInt * cells)
{
    _F_;
    DM dm = this->mesh->get_dm();
    int n_all_elems = this->mesh->get_num_all_elements();
    PetscInt elem_first, elem_last;
    DMPolytopeType polytope_type;

    if (cells == nullptr) {
        this->mesh->get_element_idx_range(elem_first, elem_last);
        n_elems_in_block = this->mesh->get_num_elements();
        PETSC_CHECK(DMPlexGetCellType(dm, elem_first, &polytope_type));
    }
    else
        PETSC_CHECK(DMPlexGetCellType(dm, cells[0], &polytope_type));

    const char * elem_type = get_elem_type(polytope_type);
    int n_nodes_per_elem = get_num_elem_nodes(polytope_type);
    const PetscInt * ordering = get_elem_node_ordering(polytope_type);
    ex_put_block(this->exoid,
                 EX_ELEM_BLOCK,
                 blk_id,
                 elem_type,
                 n_elems_in_block,
                 n_nodes_per_elem,
                 0,
                 0,
                 0);

    int * connect = new int[n_elems_in_block * n_nodes_per_elem];
    MEM_CHECK(connect);
    for (PetscInt i = 0, j = 0; i < n_elems_in_block; i++) {
        PetscInt elem_id;
        if (cells == nullptr)
            elem_id = elem_first + i;
        else
            elem_id = cells[i];
        PetscInt closure_size;
        PetscInt * closure = NULL;
        PETSC_CHECK(DMPlexGetTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
        for (PetscInt k = 0; k < n_nodes_per_elem; k++, j++) {
            PetscInt l = 2 * (closure_size - n_nodes_per_elem + ordering[k]);
            connect[j] = closure[l] - n_all_elems + 1;
        }
        PETSC_CHECK(
            DMPlexRestoreTransitiveClosure(dm, elem_id, PETSC_TRUE, &closure_size, &closure));
    }
    ex_put_conn(this->exoid, EX_ELEM_BLOCK, blk_id, connect, nullptr, nullptr);
    delete[] connect;
}

} // namespace godzilla
