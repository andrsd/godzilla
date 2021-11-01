#pragma once

#include "grids/GUnstructuredMesh.h"

/// Mesh loaded from a ExodusII file
///
class GExodusIIMesh : public GUnstructuredMesh
{
public:
    GExodusIIMesh(const InputParameters & parameters);

    virtual void create();

protected:
    /// File name with the ExodusII mesh
    const std::string & file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static InputParameters validParams();
};
