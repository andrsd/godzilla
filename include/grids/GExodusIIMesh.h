#pragma once

#include "grids/GMesh.h"

/// Mesh loaded from a ExodusII file
///
class GExodusIIMesh : public GMesh
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
