// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/MeshObject.h"

namespace godzilla {

class UnstructuredMesh;

/// Mesh loaded from a file
///
class FileMesh : public MeshObject {
protected:
    enum FileFormat { UNKNOWN, EXODUSII, GMSH } file_format;

public:
    explicit FileMesh(const Parameters & parameters);

    /// Return file name
    ///
    /// @return Name of the file
    [[nodiscard]] const std::string & get_file_name() const;

protected:
    void set_file_format(FileFormat fmt);
    Mesh * create_mesh() override;

private:
    void detect_file_format();
    UnstructuredMesh * create_from_exodus();
    UnstructuredMesh * create_from_gmsh();

    /// File name with the mesh
    std::string file_name;

public:
    static Parameters parameters();
};

} // namespace godzilla
