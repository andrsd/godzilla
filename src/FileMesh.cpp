#include "godzilla/Godzilla.h"
#include "godzilla/App.h"
#include "godzilla/FileMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/Utils.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

Parameters
FileMesh::parameters()
{
    Parameters params = UnstructuredMesh::parameters();
    params.add_required_param<std::string>("file", "The name of the file.");
    return params;
}

FileMesh::FileMesh(const Parameters & parameters) : UnstructuredMesh(parameters)
{
    _F_;

    std::filesystem::path file(get_param<std::string>("file"));
    if (file.is_absolute())
        this->file_name = file;
    else
        this->file_name = fs::path(get_app()->get_input_file_name()).parent_path() / file;

    if (!utils::path_exists(this->file_name))
        log_error(
            "Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
            this->file_name);
}

const std::string &
FileMesh::get_file_name() const
{
    _F_;
    return this->file_name;
}

} // namespace godzilla
