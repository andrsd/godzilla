// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/UnstructuredMesh.h"

namespace godzilla {

/// Mesh loaded from a file
///
class FileMesh : public UnstructuredMesh {
protected:
    enum FileFormat { UNKNOWN, EXODUSII, GMSH } file_format;

public:
    explicit FileMesh(const Parameters & parameters);

    void create() override;
    void check() override;

    /// Return file name
    ///
    /// @return Name of the file
    [[nodiscard]] const std::string & get_file_name() const;

protected:
    void set_file_format(FileFormat fmt);

private:
    void detect_file_format();
    void create_from_exodus();
    void create_from_gmsh();

    /// File name with the mesh
    std::string file_name;

public:
    static Parameters parameters();
};

} // namespace godzilla
