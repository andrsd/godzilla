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

    ///
    void set_boundary_type(DMBoundaryType bx,
                           DMBoundaryType by = DM_BOUNDARY_NONE,
                           DMBoundaryType bz = DM_BOUNDARY_NONE);

    /// Sets the type of the communication stencil
    void set_stencil_type(DMDAStencilType stype);

    /// Sets the number of grid points in the three dimensional directions
    void set_sizes(Int M, Int N = 0, Int P = 0);

    /// Sets the number of processes in each dimension
    void set_num_procs(Int m, Int n = 0, Int p = 0);

    /// Sets the size of the per-processor overlap.
    ///
    /// @param x Overlap in the x direction
    /// @param y Overlap in the y direction
    /// @param z Overlap in the z direction
    void set_overlap(Int x, Int y = 0, Int z = 0);

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
