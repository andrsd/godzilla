#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a ExodusII file
///
class ExodusIIMesh : public UnstructuredMesh
{
public:
    ExodusIIMesh(const InputParameters & parameters);

    const std::string getFileName() const;

    virtual void create();

protected:
    /// File name with the ExodusII mesh
    const std::string & file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static InputParameters validParams();
};

}
