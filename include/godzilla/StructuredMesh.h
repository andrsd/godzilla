// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "Mesh.h"

namespace godzilla {

class StructuredMesh : public Mesh {
public:
    explicit StructuredMesh(const mpi::Communicator & comm);
    explicit StructuredMesh(DM dm);

    /// Clone this structured mesh object
    ///
    /// @return Clone of this object
    StructuredMesh clone() const;

public:
    /// Creates an object that will manage the communication of one-dimensional regular array data
    /// that is distributed across one or more MPI processes.
    ///
    /// @param comm MPI communicator
    /// @param bx Type of ghost cells at the boundary the array should have, if any. Use
    ///        `DM_BOUNDARY_NONE`, `DM_BOUNDARY_GHOSTED`, or `DM_BOUNDARY_PERIODIC`.
    /// @param M Global dimension of the array (that is the number of grid points)
    /// @param dof Number of degrees of freedom per node
    /// @param s Stencil width
    static StructuredMesh
    create_1d(const mpi::Communicator comm, DMBoundaryType bx, Int M, Int dof, Int s);

    static StructuredMesh create_2d(const mpi::Communicator comm,
                                    DMBoundaryType bx,
                                    DMBoundaryType by,
                                    DMDAStencilType stencil_type,
                                    Int M,
                                    Int N,
                                    Int m,
                                    Int n,
                                    Int dof,
                                    Int s);
};

} // namespace godzilla
