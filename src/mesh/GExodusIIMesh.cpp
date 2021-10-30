#include "mesh/GExodusIIMesh.h"
#include "base/CallStack.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/exodusII_io_helper.h"
#include "base/MooseApp.h"

registerMooseObject("GodzillaApp", GExodusIIMesh);

InputParameters
GExodusIIMesh::validParams()
{
    InputParameters params = GMesh::validParams();
    params.addRequiredParam<std::string>("file", "The name of the ExodusII file.");
    return params;
}

GExodusIIMesh::GExodusIIMesh(const InputParameters & parameters) :
    GMesh(parameters),
    file_name(getParam<std::string>("file")),
    mesh(_app.comm())
{
    _F_;

    if (!MooseUtils::pathExists(this->file_name))
        godzillaError("Unable to open '", this->file_name, "' for reading. Make sure it exists and you have read permissions.");
}

void
GExodusIIMesh::create()
{
    _F_;

    ExodusII_IO exio(this->mesh);
    exio.read(this->file_name);

    ExodusII_IO_Helper & helper = exio.get_exio_helper();
    godzillaPrint(5, "Mesh: ", this->file_name);
    godzillaPrint(5, "- nodes: ", helper.num_nodes);
    godzillaPrint(5, "- elements: ", helper.num_elem);
}
