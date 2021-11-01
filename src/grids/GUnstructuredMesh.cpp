#include "grids/GUnstructuredMesh.h"
#include "base/CallStack.h"

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
