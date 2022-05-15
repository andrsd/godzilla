#include "Godzilla.h"
#include "GodzillaConfig.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "exodusII.h"

namespace godzilla {

static const unsigned int MAX_DATE_TIME = 255;

registerObject(ExodusIIOutput);

InputParameters
ExodusIIOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

ExodusIIOutput::ExodusIIOutput(const InputParameters & params) :
    FileOutput(params),
    fepi(dynamic_cast<const FEProblemInterface *>(this->problem)),
    mesh(this->problem ? dynamic_cast<const UnstructuredMesh *>(this->problem->get_mesh())
                       : nullptr),
    exoid(-1),
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
ExodusIIOutput::output_step(PetscInt stepi)
{
    _F_;
    // We only have fixed meshes, so no need to deal with a sequence of files
    set_file_name();
    godzilla_print(9, "Output to file: ", this->file_name);

    if (this->exoid == -1)
        open_file();
    if (!this->mesh_stored) {
        write_info();
        write_mesh();
    }
}

void
ExodusIIOutput::open_file()
{
    _F_;
    int cpu_word_size = sizeof(PetscReal);
    int io_word_size = sizeof(PetscReal);
    this->exoid = ex_create(this->file_name.c_str(), EX_CLOBBER, &cpu_word_size, &io_word_size);
    if (exoid == -1)
        error("Could not open file '", get_file_name(), "' for writing.");
}

void
ExodusIIOutput::write_mesh()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = this->mesh->get_dm();

    int n_nodes = this->mesh->get_num_vertices();
    int n_elems = this->mesh->get_num_elements();

    // number of element blocks
    int n_elem_blk = 0;
    ierr = DMGetLabelSize(dm, "Cell Sets", &n_elem_blk);
    check_petsc_error(ierr);
    // no cell sets defined, therefore we have one element block
    if (n_elem_blk == 0)
        n_elem_blk = 1;

    // TODO: store node sets
    int n_node_sets = 0;
    // DMGetLabelSize(dm, "Vertex Sets", &n_node_sets));

    // TODO: store side sets
    int n_side_sets = 0;
    // DMGetLabelSize(dm, "Face Sets", &n_side_sets)

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
    PetscErrorCode ierr;

    DM dm = this->mesh->get_dm();
    PetscInt dim = this->mesh->get_dimension();
    Vec coord;
    ierr = DMGetCoordinatesLocal(dm, &coord);
    check_petsc_error(ierr);
    PetscInt coord_size;
    ierr = VecGetSize(coord, &coord_size);
    check_petsc_error(ierr);
    PetscScalar * xyz;
    ierr = VecGetArray(coord, &xyz);
    check_petsc_error(ierr);

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

    ierr = VecRestoreArray(coord, &xyz);
    check_petsc_error(ierr);

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
    PetscErrorCode ierr;
    DM dm = this->mesh->get_dm();

    int n_cells_sets = 0;
    ierr = DMGetLabelSize(dm, "Cell Sets", &n_cells_sets);
    check_petsc_error(ierr);

    if (n_cells_sets > 0) {
        // TODO: write element blocks
        error("Support for mesh blocks is not implemented yet.");
    }
    else {
        int blk_id = 0;

        PetscInt elem_first, elem_last;
        ierr = DMPlexGetHeightStratum(dm, 0, &elem_first, &elem_last);
        check_petsc_error(ierr);
        int n_elems_in_block = elem_last - elem_first;

        DMPolytopeType polytope_type;
        ierr = DMPlexGetCellType(dm, elem_first, &polytope_type);
        check_petsc_error(ierr);
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
            ierr = DMPlexGetTransitiveClosure(dm, e, PETSC_TRUE, &closure_size, &closure);
            check_petsc_error(ierr);
            for (PetscInt i = 0; i < n_nodes_per_elem; i++, j++) {
                PetscInt k = 2 * (closure_size - n_nodes_per_elem + ordering[i]);
                connect[j] = closure[k] - n_elems_in_block + 1;
            }
            ierr = DMPlexRestoreTransitiveClosure(dm, e, PETSC_TRUE, &closure_size, &closure);
            check_petsc_error(ierr);
        }

        ex_put_conn(this->exoid, EX_ELEM_BLOCK, blk_id, connect, nullptr, nullptr);

        delete[] connect;
    }
}

void
ExodusIIOutput::write_node_sets()
{
    _F_;
}

void
ExodusIIOutput::write_face_sets()
{
    _F_;
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
