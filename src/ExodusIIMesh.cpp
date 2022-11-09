#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"
#include "exodusIIcpp.h"

namespace godzilla {

REGISTER_OBJECT(ExodusIIMesh);

Parameters
ExodusIIMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
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

    // Ideally we would like to use DMPlexCreateExodus here and get rid of the above
    // DMPlexCreateExodusFromFile, so that we don't open the same file twice. For some reason,
    // that just doesn't work even though this is just exactly what DMPlexCreateExodusFromFile
    // is doing :confused:
    exodusIIcpp::File f(this->file_name, exodusIIcpp::FileAccess::READ);
    if (f.is_opened()) {
        auto blk_names = f.read_block_names();
        for (auto & it : blk_names) {
            auto name = it.second;
            if (name.empty()) {
                name = std::to_string(it.first);
                create_cell_set(it.first, name);
            }
            else
                create_cell_set(it.first, it.second);
        }

        auto sideset_names = f.read_side_set_names();
        create_face_set_labels(sideset_names);

        f.close();
    }
}

} // namespace godzilla
