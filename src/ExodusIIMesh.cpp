#include "Godzilla.h"
#include "ExodusIIMesh.h"
#include "CallStack.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Error.h"
#include "Edge.h"
#include "Tri.h"
#include "Quad.h"
#include "Tetra.h"
#include "Hex.h"
#include "Utils.h"
#include <exodusII.h>

// TODO: error checking on all exodusII functions

namespace godzilla {

enum EElemType { INVALID, EDGE, TRIANGLE, QUAD, HEX, TETRA, PRISM, PYRAMID };

registerObject(ExodusIIMesh);

InputParameters
ExodusIIMesh::validParams()
{
    InputParameters params = Mesh::validParams();
    params.add_required_param<std::string>("file", "The name of the ExodusII file.");
    return params;
}

ExodusIIMesh::ExodusIIMesh(const InputParameters & params) :
    Mesh(params),
    file_name(get_param<std::string>("file"))
{
    _F_;
}

const std::string &
ExodusIIMesh::get_file_name() const
{
    _F_;
    return this->file_name;
}

void
ExodusIIMesh::create_dm()
{
    _F_;
    if (utils::path_exists(this->file_name)) {
        PetscErrorCode ierr;
        ierr = DMPlexCreateExodusFromFile(comm(), this->file_name.c_str(), PETSC_TRUE, &this->dm);
        checkPetscError(ierr);
    }
    else
        log_error("Unable to open '",
                  this->file_name,
                  "' for reading. Make sure it exists and you have read permissions.");
}

} // namespace godzilla
