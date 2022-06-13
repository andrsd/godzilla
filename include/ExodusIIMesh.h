#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a ExodusII file
///
class ExodusIIMesh : public UnstructuredMesh {
public:
    ExodusIIMesh(const InputParameters & parameters);

    const std::string get_file_name() const;

protected:
    virtual void create_dm() override;
    void read_side_sets(int exoid, int n_side_sets);

    /// File name with the ExodusII mesh
    const std::string & file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
