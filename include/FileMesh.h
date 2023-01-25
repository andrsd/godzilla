#pragma once

#include "UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a file
///
class FileMesh : public UnstructuredMesh {
public:
    explicit FileMesh(const Parameters & parameters);

    /// Return file name
    ///
    /// @return Name of the file
    NO_DISCARD const std::string & get_file_name() const;

protected:
    /// File name with the mesh
    std::string file_name;
    /// Create faces and edges in the mesh
    const PetscBool interpolate;

public:
    static Parameters parameters();
};

} // namespace godzilla
