#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a ExodusII file
///
class ExodusIIMesh : public UnstructuredMesh {
public:
    ExodusIIMesh(const Parameters & parameters);

    const std::string get_file_name() const;

protected:
    virtual void create_dm() override;

    /// File name with the ExodusII mesh
    const std::string & file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static Parameters valid_params();
};

} // namespace godzilla
