#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"
#include "exodusII.h"

namespace godzilla {

registerObject(ExodusIIMesh);

InputParameters
ExodusIIMesh::valid_params()
{
    InputParameters params = UnstructuredMesh::valid_params();
    params.add_required_param<std::string>("file", "The name of the ExodusII file.");
    return params;
}

ExodusIIMesh::ExodusIIMesh(const InputParameters & parameters) :
    UnstructuredMesh(parameters),
    file_name(get_param<std::string>("file")),
    interpolate(PETSC_TRUE)
{
    _F_;

    if (!utils::path_exists(this->file_name))
        log_error(
            "Unable to open '%s' for reading. Make sure it exists and you have read permissions.",
            this->file_name);
}

const std::string
ExodusIIMesh::get_file_name() const
{
    _F_;
    return this->file_name;
}

void
ExodusIIMesh::create_dm()
{
    _F_;
    PETSC_CHECK(DMPlexCreateExodusFromFile(get_comm(),
                                           this->file_name.c_str(),
                                           this->interpolate,
                                           &this->dm));

    int cpu_word_size = sizeof(PetscReal);
    int io_word_size = 0;
    float version;
    int exoid = -1;

    exoid = ex_open(this->file_name.c_str(), EX_READ, &cpu_word_size, &io_word_size, &version);
    if (exoid != -1) {
        // Ideally we would like to use DMPlexCreateExodus here and get rid of the above
        // DMPlexCreateExodusFromFile, so that we don't open the same file twice. For some reason,
        // that just doesn't work even though this is just exactly what DMPlexCreateExodusFromFile
        // is doing :confused:

        char title[MAX_LINE_LENGTH + 1];
        memset(title, 0, sizeof(title));
        int n_dim, n_nodes, n_elem, n_elem_blk, n_node_sets, n_side_sets;
        ex_get_init(exoid,
                    title,
                    &n_dim,
                    &n_nodes,
                    &n_elem,
                    &n_elem_blk,
                    &n_node_sets,
                    &n_side_sets);

        read_cell_sets(exoid, n_elem_blk);
        read_side_sets(exoid, n_side_sets);

        ex_close(exoid);
    }
}

void
ExodusIIMesh::read_cell_sets(int exoid, int n_blk_sets)
{
    _F_;
    int * ids = new int[n_blk_sets];
    ex_get_ids(exoid, EX_ELEM_BLOCK, ids);

    std::string cell_set_name;
    char name[MAX_STR_LENGTH + 1];
    for (int i = 0; i < n_blk_sets; i++) {
        ex_get_name(exoid, EX_ELEM_BLOCK, ids[i], name);
        if (strnlen(name, MAX_STR_LENGTH) == 0)
            cell_set_name = std::to_string(ids[i]);
        else
            cell_set_name = name;
        create_cell_set(ids[i], cell_set_name);
    }

    delete[] ids;
}

void
ExodusIIMesh::read_side_sets(int exoid, int n_side_sets)
{
    _F_;
    int * ids = new int[n_side_sets];
    ex_get_ids(exoid, EX_SIDE_SET, ids);

    char name[MAX_STR_LENGTH + 1];
    for (int i = 0; i < n_side_sets; i++) {
        ex_get_name(exoid, EX_SIDE_SET, ids[i], name);
        this->face_set_names[ids[i]] = name;
    }

    delete[] ids;
}

} // namespace godzilla
