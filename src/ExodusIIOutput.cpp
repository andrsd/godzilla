#include "Godzilla.h"
#include "GodzillaConfig.h"
#include "CallStack.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Postprocessor.h"
#include "exodusII.h"
#include <assert.h>

namespace godzilla {

static const unsigned int MAX_DATE_TIME = 255;

registerObject(ExodusIIOutput);

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

/// Add variable names into the list of names
///
/// @param nc Number of components
/// @param name Field name
/// @param nums If `true`, component names will be numbers, otherwise they will be 'x', 'y', 'z'
/// @param var_names Vector of variable names that will be added into
static void
add_var_names(PetscInt nc,
              const std::string & name,
              bool nums,
              std::vector<std::string> & var_names)
{
    _F_;
    if (nc == 1)
        var_names.push_back(name);
    else {
        if (nums) {
            for (PetscInt c = 0; c < nc; c++) {
                std::ostringstream oss;
                internal::fprintf(oss, "%s_%d", name, c);
                var_names.push_back(oss.str());
            }
        }
        else {
            assert(nc <= 3);
            const char * comp_name[] = { "x", "y", "z" };
            for (PetscInt c = 0; c < nc; c++) {
                std::ostringstream oss;
                internal::fprintf(oss, "%s_%s", name, comp_name[c]);
                var_names.push_back(oss.str());
            }
        }
    }
}

InputParameters
ExodusIIOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    params.add_param<std::vector<std::string>>(
        "variables",
        "List of variables to be stored. If not specified, all variables will be stored.");
    return params;
}

ExodusIIOutput::ExodusIIOutput(const InputParameters & params) :
    FileOutput(params),
    variable_names(get_param<std::vector<std::string>>("variables")),
    fepi(dynamic_cast<const FEProblemInterface *>(this->problem)),
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
ExodusIIOutput::create()
{
    _F_;
    FileOutput::create();

    auto flds = this->fepi->get_field_names();
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
    if (this->fepi == nullptr)
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
    DM dm = this->mesh->get_dm();

    int n_nodes = this->mesh->get_num_vertices();
    int n_elems = this->mesh->get_num_elements();

    // number of element blocks
    int n_elem_blk = 0;
    PETSC_CHECK(DMGetLabelSize(dm, "Cell Sets", &n_elem_blk));
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    int n_node_sets;
    PETSC_CHECK(DMGetLabelSize(dm, "Vertex Sets", &n_node_sets));

    int n_side_sets;
    PETSC_CHECK(DMGetLabelSize(dm, "Face Sets", &n_side_sets));

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

void
ExodusIIOutput::write_elements()
{
    _F_;
    DM dm = this->mesh->get_dm();

    int n_cells_sets = 0;
    PETSC_CHECK(DMGetLabelSize(dm, "Cell Sets", &n_cells_sets));

    if (n_cells_sets > 0) {
        // TODO: write element blocks
        error("Support for mesh blocks is not implemented yet.");
    }
    else {
        int blk_id = 0;

        PetscInt elem_first, elem_last;
        this->mesh->get_element_idx_range(elem_first, elem_last);
        int n_elems_in_block = elem_last - elem_first;

        DMPolytopeType polytope_type;
        PETSC_CHECK(DMPlexGetCellType(dm, elem_first, &polytope_type));
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

        for (PetscInt e = elem_first, j = 0; e < elem_last; e++) {
            PetscInt closure_size;
            PetscInt * closure = NULL;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, e, PETSC_TRUE, &closure_size, &closure));
            for (PetscInt i = 0; i < n_nodes_per_elem; i++, j++) {
                PetscInt k = 2 * (closure_size - n_nodes_per_elem + ordering[i]);
                connect[j] = closure[k] - n_elems_in_block + 1;
            }
            PETSC_CHECK(DMPlexRestoreTransitiveClosure(dm, e, PETSC_TRUE, &closure_size, &closure));
        }

        ex_put_conn(this->exoid, EX_ELEM_BLOCK, blk_id, connect, nullptr, nullptr);

        delete[] connect;
    }
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

    int n_node_sets;
    PETSC_CHECK(DMGetLabelSize(dm, "Vertex Sets", &n_node_sets));

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

    int n_side_sets;
    PETSC_CHECK(DMGetLabelSize(dm, "Face Sets", &n_side_sets));
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
            elem_list[i] = el + 1;

            PETSC_CHECK(
                DMPlexRestoreTransitiveClosure(dm, faces[i], PETSC_FALSE, &num_points, &points));

            // Side
            points = NULL;
            PETSC_CHECK(DMPlexGetTransitiveClosure(dm, el, PETSC_TRUE, &num_points, &points));

            for (PetscInt j = 1; j < num_points; ++j) {
                if (points[j * 2] == faces[i]) {
                    side_list[i] = j;
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
ExodusIIOutput::write_all_variable_names()
{
    _F_;

    this->nodal_var_fids.clear();
    this->elem_var_fids.clear();
    std::vector<std::string> nodal_var_names;
    std::vector<std::string> elem_var_names;
    for (auto & name : this->field_var_names) {
        PetscInt fid = this->fepi->get_field_id(name);
        PetscInt nc = this->fepi->get_field_num_components(fid);
        PetscInt order = this->fepi->get_field_order(fid);
        if (order == 0) {
            add_var_names(nc, name, nc > 3, elem_var_names);
            this->elem_var_fids.push_back(fid);
        }
        else {
            add_var_names(nc, name, nc > 3, nodal_var_names);
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
    // TODO: write elemental variables

    PETSC_CHECK(VecRestoreArrayRead(sln, &sln_vals));

    PETSC_CHECK(DMRestoreLocalVector(dm, &sln));
}

void
ExodusIIOutput::write_nodal_variables(const PetscScalar * sln)
{
    _F_;

    PetscErrorCode ierr;
    DM dm = this->problem->get_dm();
    PetscInt n_elems = this->mesh->get_num_elements();
    PetscInt first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(dm, this->mesh->get_dimension(), &first, &last));

    PetscSection section;
    PETSC_CHECK(DMGetLocalSection(dm, &section));

    for (PetscInt n = first; n < last; n++) {
        int exo_var_id = 1;
        for (std::size_t i = 0; i < this->nodal_var_fids.size(); i++) {
            PetscInt fid = this->nodal_var_fids[i];

            PetscInt offset;
            PETSC_CHECK(PetscSectionGetFieldOffset(section, n, fid, &offset));

            PetscInt nc = this->fepi->get_field_num_components(fid);
            for (PetscInt c = 0; c <= nc; c++, exo_var_id++) {
                int exo_idx = n - n_elems + 1;
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

} // namespace godzilla
