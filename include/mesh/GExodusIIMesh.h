#pragma once

#include "mesh/GMesh.h"
#include "libmesh/distributed_mesh.h"

/// Mesh loaded from a ExodusII file
///
class GExodusIIMesh : public GMesh
{
public:
    GExodusIIMesh(const InputParameters & parameters);

    virtual void create();

protected:
    /// File name with the ExodusII mesh
    const std::string file_name;
    /// libMesh mesh
    libMesh::DistributedMesh mesh;

public:
    static InputParameters validParams();
};
