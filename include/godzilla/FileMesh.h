#pragma once

#include "godzilla/UnstructuredMesh.h"

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

private:
    /// File name with the mesh
    std::string file_name;

public:
    static Parameters parameters();
};

} // namespace godzilla
