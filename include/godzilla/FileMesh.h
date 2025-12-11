// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Qtr.h"
#include "godzilla/UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a file
///
class FileMesh : public Object {
public:
    enum FileFormat { UNKNOWN, EXODUSII, GMSH } file_format;

    explicit FileMesh(const Parameters & pars);

    /// Return file name
    ///
    /// @return Name of the file
    const String & get_file_name() const;

    /// Get mesh file format
    ///
    /// @return Mesh file format
    FileFormat get_file_format() const;

    Qtr<UnstructuredMesh> create_mesh();

protected:
    void set_file_format(FileFormat fmt);

private:
    void detect_file_format();
    Qtr<UnstructuredMesh> create_from_exodus();
    Qtr<UnstructuredMesh> create_from_gmsh();

    /// File name with the mesh
    String file_name;

public:
    static Parameters parameters();
};

} // namespace godzilla
