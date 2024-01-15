// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"

namespace godzilla {

class PCJacobi : public Preconditioner {
public:
    enum Type {
        DIAGONAL = PC_JACOBI_DIAGONAL,
        ROWMAX = PC_JACOBI_ROWMAX,
        ROWSUM = PC_JACOBI_ROWSUM
    };

    PCJacobi();
    PCJacobi(PC pc);

    /// Creates a preconditioner
    ///
    /// @param comm MPI communicator
    void create(MPI_Comm comm);

    /// Causes the preconditioner to use either the diagonal, the maximum entry in each row, or the
    /// sum of rows entries for the diagonal preconditioner
    ///
    /// @param type How the diagonal matrix is produced
    void set_type(Type type);

    /// Gets how the diagonal matrix is produced for the preconditioner
    ///
    /// @return How the diagonal matrix is produced
    Type get_type() const;

    /// Determines if the preconditioner checks for zero diagonal terms
    ///
    /// @return `true` if check is on, `false` otherwise
    bool get_fix_diagonal() const;

    /// Determines if the preconditioner uses the absolute values of the diagonal divisors in
    /// the preconditioner
    ///
    /// @return `true` if checking is on, `false` otherwise
    bool get_use_abs() const;

    /// Check for zero values on the diagonal and replace them with 1.0
    ///
    /// @param flag `true` turn the fixing on, `false` otherwise
    void set_fix_diagonal(bool flag);

    /// Causes the preconditioner to use the absolute values of the diagonal divisors in the
    /// preconditioner
    ///
    /// @param flag `true` to use the absolute value, `false` otherwise
    void set_use_abs(bool flag);
};

} // namespace godzilla
