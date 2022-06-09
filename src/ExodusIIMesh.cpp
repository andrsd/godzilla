#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Utils.h"
#include "petscdmplex.h"

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
}

} // namespace godzilla
