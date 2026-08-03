// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Vector.h"
#include "petscvec.h"

namespace godzilla {

/// Wrapper for a PETSc vector scatter context, which manages communication between vectors.
///
/// @see https://petsc.org/release/manualpages/Vec/VecScatter/
class VectorScatter : public PetscObjectWrapper<VecScatter> {
public:
    /// Default constructor creating a null VectorScatter.
    VectorScatter();

    /// Construct VectorScatter from an existing PETSc VecScatter object.
    ///
    /// @param vs The PETSc VecScatter object
    explicit VectorScatter(VecScatter vs);

    /// Begins the scattering operation.
    ///
    /// @param x The source vector
    /// @param y The destination vector
    /// @param addv Either `INSERT_VALUES` or `ADD_VALUES`
    /// @param mode Communication mode (e.g. `SCATTER_FORWARD` or `SCATTER_REVERSE`)

    void begin(const Vector & x, Vector & y, InsertMode addv, ScatterMode mode) const;

    /// Ends the scattering operation.
    ///
    /// @param x The source vector
    /// @param y The destination vector
    /// @param addv Either `INSERT_VALUES` or `ADD_VALUES`
    /// @param mode Communication mode (e.g. `SCATTER_FORWARD` or `SCATTER_REVERSE`)
    void end(const Vector & x, Vector & y, InsertMode addv, ScatterMode mode) const;

    /// Creates a copy of the scatter context.
    ///
    /// @return The copied raw PETSc VecScatter object
    VecScatter copy() const;

    /// Determine if a vector scatter context is operating in "merged" mode.
    ///
    /// @return `true` if merged, `false` otherwise
    bool get_merged() const;

    /// Retrieves the type name of the VectorScatter object.
    ///
    /// @return The type name as a string
    String get_type() const;

    /// Sets the specific implementation type for the scatter.
    ///
    /// @param type The PETSc scatter type name (e.g., VECSCATTERSEQ, VECSCATTERMPI)
    void set_type(VecScatterType type);

    /// Explicitly sets up the internal structures of the scatter context.
    void set_up();

    /// Views or inspects the communication pattern of the scatter context.
    ///
    /// @param viewer The PETSc viewer to output to
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

public:
    /// Creates a scatter context to move data from vector x (at indices ix) to vector y (at indices
    /// iy).
    ///
    /// @param x The source vector
    /// @param ix The source index set
    /// @param y The destination vector
    /// @param iy The destination index set
    /// @return The created VectorScatter
    static VectorScatter
    create(const Vector & x, const IndexSet & ix, const Vector & y, const IndexSet & iy);

    /// Creates a scatter context that gathers a parallel vector into a sequential vector on
    /// all processes.
    ///
    /// @param in The input parallel vector
    /// @return A tuple containing the created VectorScatter and the destination sequential vector
    static std::tuple<VectorScatter, Vector> create_to_all(const Vector & in);

    /// Creates a scatter context that gathers a parallel vector into a sequential vector on rank 0.
    ///
    /// @param in The input parallel vector
    /// @return A tuple containing the created VectorScatter and the destination sequential vector
    static std::tuple<VectorScatter, Vector> create_to_zero(const Vector & in);
};

} // namespace godzilla
