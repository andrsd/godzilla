#include "problems/GProblem.h"
#include "base/CallStack.h"

InputParameters
GProblem::validParams()
{
    InputParameters params = Problem::validParams();
    params.addPrivateParam<std::string>("_moose_base", "gproblem");
    return params;
}

GProblem::GProblem(const InputParameters & parameters) :
    Problem(parameters),
    GPrintInterface(this)
{
    _F_;
}

MPI_Comm
GProblem::comm()
{
    return libMesh::ParallelObject::comm().get();
}

void
GProblem::init()
{
    _F_;
}
