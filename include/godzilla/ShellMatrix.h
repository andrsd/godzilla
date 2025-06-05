// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Matrix.h"
#include "godzilla/Delegate.h"
#include "petscmat.h"
#include <vector>

namespace godzilla {

/// A matrix to be used for user-defined matrix type – perhaps matrix-free.
class ShellMatrix : public Matrix {
public:
    ShellMatrix();

    explicit ShellMatrix(Mat m);

    /// Creates a new matrix of type `MATSHELL` for use with a user-defined private data storage
    /// format.
    ///
    /// @param comm The MPI communicator
    /// @param m Number of local rows (or PETSC_DECIDE to have calculated if M is given)
    /// @param n Number of local columns (or PETSC_DECIDE to have calculated if N is given)
    /// @param M Number of global rows (may be PETSC_DETERMINE to have calculated if m is given)
    /// @param N Number of global columns (may be PETSC_DETERMINE to have calculated if n is
    /// given)
    void create(MPI_Comm comm, Int m, Int n, Int M, Int N);

    /// Set a matrix operation
    ///
    /// @param op The matrix operation
    /// @param g The function that provides the operation
    void set_operation(MatOperation op, void (*g)(void));

    /// Set a matrix operation
    ///
    /// @param op The matrix operation
    /// @param instance The instance of the class
    /// @param method The method of the class
    template <class T, typename... ARGS>
    void
    set_operation(MatOperation op, T * instance, void (T::*method)(ARGS...))
    {
        if (op == MATOP_MULT) {
            this->mult_delegate.bind(instance, method);
            PETSC_CHECK(MatShellSetOperation(*this,
                                             MATOP_MULT,
                                             (void (*)()) ShellMatrix::invoke_matmult_op_delegate));
        }
        else {
            throw Exception("Unsupported operation: {}", static_cast<int>(op));
        }
    }

private:
    /// The delegate for the matrix-vector multiplication operation
    Delegate<void(Matrix & A, Vector & x_vec, Vector & y_vec)> mult_delegate;

public:
    static ErrorCode invoke_matmult_op_delegate(Mat matrix, Vec vector, Vec action);
};

} // namespace godzilla
