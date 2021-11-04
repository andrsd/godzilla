#include "Godzilla.h"
#include "grids/GUnstructuredMesh.h"
#include "base/CallStack.h"


namespace godzilla {

InputParameters
GUnstructuredMesh::validParams()
{
    InputParameters params = GGrid::validParams();
    return params;
}

GUnstructuredMesh::GUnstructuredMesh(const InputParameters & parameters) :
    GGrid(parameters)
{
    _F_;
}

}
