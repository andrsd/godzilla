// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Matrix.h"
#include "godzilla/PetscObjectWrapper.h"
#include "petscpc.h"

namespace godzilla {

/// Preconditioner - wrapper around PETSc PC
class Preconditioner : public PetscObjectWrapper<PC> {
public:
    Preconditioner();
    Preconditioner(PC pc);

    /// Creates a preconditioner
    ///
    /// @param comm MPI communicator
    void create(MPI_Comm comm);

    /// Destroys the preconditioner
    void destroy();

    /// Builds PC for a particular preconditioner type
    ///
    /// @param type a known method, see PCType for possible values
    void set_type(const std::string & type);

    /// Gets the preconditioner type (as a string)
    ///
    /// @return name of preconditioner method
    std::string get_type() const;

    /// Resets the preconditioner and removes any allocated Vectors and Matrices
    void reset();

    /// Prepares the preconditioner for the use
    void set_up();

    /// Sets the matrix associated with the linear system and a (possibly) different one associated
    /// with the preconditioner.
    ///
    /// @param A The matrix that defines the linear system
    /// @param P The matrix to be used in constructing the preconditioner, usually the same as Amat.
    void set_operators(const Matrix & A, const Matrix & P);

    /// Prints information about the PC
    ///
    /// @param viewer Viewer
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    /// Applies the preconditioner to a vector
    ///
    /// @param x Input vector
    /// @param y Output vector
    void apply(const Vector & x, Vector & y) const;
};

} // namespace godzilla
