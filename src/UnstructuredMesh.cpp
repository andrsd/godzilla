#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"


namespace godzilla {

InputParameters
UnstructuredMesh::validParams()
{
    InputParameters params = GGrid::validParams();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const InputParameters & parameters) :
    GGrid(parameters)
{
    _F_;
}

}
