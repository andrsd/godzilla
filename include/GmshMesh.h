#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a Gmsh file
///
class GmshMesh : public UnstructuredMesh {
public:
    explicit GmshMesh(const Parameters & parameters);

    /// Return ExodusII file name
    ///
    /// @return Name of the ExodusII file
    NO_DISCARD const std::string & get_file_name() const;

protected:
    void create_dm() override;

    /// File name with the ExodusII mesh
    std::string file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
