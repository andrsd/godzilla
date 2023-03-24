#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "App.h"
#include "CallStack.h"
#include "exodusIIcpp.h"

namespace godzilla {

REGISTER_OBJECT(ExodusIIMesh);

Parameters
ExodusIIMesh::parameters()
{
    Parameters params = FileMesh::parameters();
    return params;
}

ExodusIIMesh::ExodusIIMesh(const Parameters & parameters) : FileMesh(parameters)
{
    _F_;
}

void
ExodusIIMesh::create_dm()
{
    _F_;
    TIMED_EVENT(9, "MeshLoad", "Loading mesh '{}'", this->file_name);
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

        std::map<Int, std::string> sideset_names;
        for (auto it : f.read_side_set_names())
            sideset_names[it.first] = it.second;
        create_face_set_labels(sideset_names);

        f.close();
    }
}

} // namespace godzilla
