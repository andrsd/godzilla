#include "godzilla/Godzilla.h"
#include "godzilla/ExodusIIMesh.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"
#include "exodusIIcpp/exodusIIcpp.h"

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
ExodusIIMesh::create()
{
    _F_;
    UnstructuredMesh::create();

    // Ideally we would like to use DMPlexCreateExodus here and get rid of the above
    // DMPlexCreateExodusFromFile, so that we don't open the same file twice. For some reason,
    // that just doesn't work even though this is just exactly what DMPlexCreateExodusFromFile
    // is doing :confused:
    exodusIIcpp::File f(get_file_name(), exodusIIcpp::FileAccess::READ);
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

        for (auto it : f.read_side_set_names())
            set_face_set_name(it.first, it.second);

        f.close();
    }
}

DM
ExodusIIMesh::create_dm()
{
    _F_;
    TIMED_EVENT(9, "MeshLoad", "Loading mesh '{}'", get_file_name());
    DM dm;
    PETSC_CHECK(DMPlexCreateExodusFromFile(get_comm(), get_file_name().c_str(), PETSC_TRUE, &dm));
    return dm;
}

} // namespace godzilla
