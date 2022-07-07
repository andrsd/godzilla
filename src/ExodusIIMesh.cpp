#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"
#include "exodusII.h"

namespace godzilla {

// NOTE: When there is an ExodusIO module, this method would go there
static std::map<int, std::string>
read_name_map(int exoid, int n, ex_entity_type obj_type)
{
    _F_;
    std::map<int, std::string> map_names;
    int * ids = new int[n];
    ex_get_ids(exoid, obj_type, ids);
    char name[MAX_STR_LENGTH + 1];
    for (int i = 0; i < n; i++) {
        ex_get_name(exoid, obj_type, ids[i], name);
        std::string str_name;
        if (strnlen(name, MAX_STR_LENGTH) == 0)
            str_name = std::to_string(ids[i]);
        else
            str_name = name;
        map_names[ids[i]] = str_name;
    }
    delete[] ids;
    return map_names;
}

REGISTER_OBJECT(ExodusIIMesh);

Parameters
ExodusIIMesh::valid_params()
{
    Parameters params = UnstructuredMesh::valid_params();
    params.add_required_param<std::string>("file", "The name of the ExodusII file.");
    return params;
}

ExodusIIMesh::ExodusIIMesh(const Parameters & parameters) :
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
    lprintf(9, "Loading mesh '%s'", this->file_name);
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

        // cell sets
        std::map<int, std::string> cell_set_names = read_name_map(exoid, n_elem_blk, EX_ELEM_BLOCK);
        for (auto & it : cell_set_names)
            create_cell_set(it.first, it.second);

        // side sets
        std::map<int, std::string> side_set_names = read_name_map(exoid, n_side_sets, EX_SIDE_SET);
        create_face_set_labels(side_set_names);

        ex_close(exoid);
    }
}

} // namespace godzilla
