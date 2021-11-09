#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"


namespace godzilla {

InputParameters
UnstructuredMesh::validParams()
{
    InputParameters params = Grid::validParams();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const InputParameters & parameters) :
    Grid(parameters)
{
    _F_;
}

}
