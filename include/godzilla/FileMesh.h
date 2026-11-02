// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Object.h"
#include "godzilla/Qtr.h"
#include "godzilla/UnstructuredMesh.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

/// Object for loading meshes from files
///
class FileMesh : public Object {
public:
    enum FileFormat { UNKNOWN, EXODUSII, GMSH } file_format;

    /// Construct a FileMesh object
    ///
    /// @param pars Parameters to build the object
    explicit FileMesh(const Parameters & pars);

    /// Return file name
    ///
    /// @return Name of the file
    fs::path get_file_name() const;

    /// Return distributed file name for given rank
    ///
    /// @param rank MPI rank
    /// @return Name of the distributed file for rank
    fs::path get_distributed_file_name(int rank) const;

    /// Get mesh file format
    ///
    /// @return Mesh file format
    FileFormat get_file_format() const;

    /// Is the mesh pre-distributed
    ///
    /// @return true if the mesh is distributed, false otherwise
    bool is_distributed() const;

    /// Create UnstructuredMesh object
    ///
    /// @return UnstructuredMesh object
    Qtr<UnstructuredMesh> create_mesh();

protected:
    void set_file_format(FileFormat fmt);

private:
    void detect_file_format();
    Qtr<UnstructuredMesh> create_from_exodus();
    Qtr<UnstructuredMesh> create_from_gmsh();

    /// File name with the mesh
    fs::path file_name_;
    /// Interpolate the mesh
    bool interpolate_;
    /// Is the mesh pre-distributed
    bool distributed_;

public:
    static Parameters parameters();
};

} // namespace godzilla
