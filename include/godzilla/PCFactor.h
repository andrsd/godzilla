// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"
#include "godzilla/Types.h"

namespace godzilla {

class PCFactor : public Preconditioner {
public:
    enum Type { ICC, ILU, LU, CHOLESKY };

    enum MatSolverType {
        SUPERLU,
        SUPERLU_DIST,
        STRUMPACK,
        UMFPACK,
        CHOLMOD,
        KLU,
        ELEMENTAL,
        SCALAPACK,
        ESSL,
        LUSOL,
        MUMPS,
        MKL_PARDISO,
        MKL_CPARDISO,
        PASTIX,
        MATLAB,
        PETSC,
        BAS,
        CUSPARSE,
        CUDA,
#if PETSC_VERSION_GE(3, 18, 0)
        HIPSPARSE,
        HIP,
#endif
        KOKKOS,
        SPQR
    };

    enum MatShiftType {
        /// do not shift the matrix diagonal entries
        NONE = MAT_SHIFT_NONE,
        /// shift the entries to be non-zero
        NONZERO = MAT_SHIFT_NONZERO,
        /// shift the entries to force the factorization to be positive definite
        POSITIVE_DEFINITE = MAT_SHIFT_POSITIVE_DEFINITE,
        /// only shift the factors inside the small dense diagonal blocks of the matrix, for
        /// example with `MATBAIJ`
        INBLOCKS = MAT_SHIFT_INBLOCKS
    };

    enum MatOrderingType {
        NATURAL,
        ND,
        ONE_WD,
        RCM,
        QMD,
        ROW_LENGTH,
        WBM,
        SPECTRAL,
        AMD,
        METISND,
        NATURAL_OR_ND,
        EXTERNAL
    };

    PCFactor();
    PCFactor(PC pc);

    /// Set the type of preconditioner
    ///
    /// @param type Preconditioner type
    void set_type(Type type);

    /// Get the precoditioner type
    ///
    /// @return Preconditioner type
    Type get_type();

    /// Determines if all diagonal matrix entries are treated as level 0 fill even if there is no
    /// non-zero location
    ///
    /// @return `true` if all diagonal matrix entries are treated as level 0, `false` otherwise
    bool get_allow_diagonal_fill() const;

    /// Gets the number of levels of fill to use
    ///
    /// @return Number of levels of fill
    Int get_levels() const;

    /// Gets the solver package that is used to perform the factorization
    ///
    /// @return The solver package that is used to perform the factorization
    MatSolverType get_mat_solver_type() const;

    /// Gets the tolerance used to define a zero pivot
    ///
    /// @return How much to shift the diagonal entry
    Real get_shift_amount() const;

    /// Gets the type of shift, if any, done when a zero pivot is detected
    MatShiftType get_shift_type() const;

    /// Determines if an in-place factorization is being used
    ///
    /// @return `true` if an in-place factorization is being used, `false` otherwise
    bool get_use_in_place() const;

    /// Gets the tolerance used to define a zero pivot
    ///
    /// @return The tolerance that defines a zero pivot
    Real get_zero_pivot() const;

    /// Reorders rows/columns of matrix to remove zeros from diagonal
    ///
    /// @param tol Diagonal entries smaller than this in absolute value are considered zero
    void reorder_for_nonzero_diagonal(Real tol);

    /// Causes all diagonal matrix entries to be treated as level 0 fill even if there is no
    /// non-zero location.
    ///
    /// @param flag `true` to turn on, `false` to turn off
    void set_allow_diagonal_fill(bool flag);

    /// Determines when column pivoting is done during matrix factorization. For PETSc dense
    /// matrices column pivoting is always done, for PETSc sparse matrices it is never done.
    /// For the MATLAB and MATSOLVERSUPERLU factorization this is used.
    ///
    /// @param dt_col `0.0` implies no pivoting, `1.0` complete pivoting (slower, requires more
    /// memory but more stable)
    void set_column_pivot(Real dt_col);

    /// The preconditioner will use an PCILU based on a drop tolerance.
    ///
    /// @param dt The drop tolerance, try from `1.e-10` to `0.1`
    /// @param dt_col Tolerance for column pivot, good values `[0.1 to 0.01]`
    /// @param max_row_count The max number of nonzeros allowed in a row, best value depends on the
    /// number of nonzeros in row of original matrix
    ///
    /// NOTE: There are NO default values for the 3 parameters, you must set them with reasonable
    /// values for your matrix.
    void set_drop_tolerance(Real dt, Real dt_col, Int max_row_count);

    /// Indicate the amount of fill you expect in the factored matrix, fill = number nonzeros in
    /// factor/number nonzeros in original matrix.
    ///
    /// @param fill Amount of expected fill
    void set_fill(Real fill);

    /// Sets the number of levels of fill to use
    ///
    /// @param levels Number of levels of fill
    void set_levels(Int levels);

    /// Sets the ordering routine (to reduce fill) to be used in the `LU`, `CHOLESKY`, `ILU`, or
    /// `ICC` preconditioners
    ///
    /// @param ordering The matrix ordering name
    void set_mat_ordering_type(MatOrderingType ordering);

    /// Sets the solver package that is used to perform the factorization
    ///
    /// @param type Matrix solver type
    void set_mat_solver_type(MatSolverType type);

    /// Sets if pivoting is done while factoring each block with MATBAIJ or MATSBAIJ matrices
    ///
    /// @param pivot `true` to turn pivoting on, `false` otherwise
    void set_pivot_in_blocks(bool pivot);

    /// When matrices with different non-zero structure are factored, this causes later ones to use
    /// the fill ratio computed in the initial factorization.
    ///
    /// @param flag `true` to turn reuse on, `false` to turn it off
    void set_reuse_fill(bool flag);

    /// When similar matrices are factored, this causes the ordering computed in the first factor
    /// to be used for all following factors.
    ///
    /// @param flag `true` to turn reuse on, `false` to turn it off
    void set_reuse_ordering(bool flag);

    /// Adds a quantity to the diagonal of the matrix during numerical factorization, thus the
    /// matrix has nonzero pivots
    ///
    /// @param shift_amount Amount of shift or `PETSC_DECIDE` for the default
    void set_shift_amount(Real shift_amount);

    /// Adds a particular type of quantity to the diagonal of the matrix during numerical
    /// factorization, thus the matrix has nonzero pivots
    ///
    /// @param type Type of shift
    void set_shift_type(MatShiftType type);

    /// Can be called after KSPSetOperators() or PCSetOperators(), causes MatGetFactor() to be
    /// called so then one may set the options for that particular factorization object.
    ///
    /// NOTE: After you have called this function (which has to be after the KSPSetOperators() or
    /// PCSetOperators()) you can call PCFactorGetMatrix() and then set factor options on that
    /// matrix. This function raises an error if the requested combination of solver package and
    /// matrix type is not supported.
    void set_up_mat_solver_type();

    /// Tells the preconditioner to do an in-place factorization.
    ///
    /// @param flg `true` to enable, `false` to disable
    void set_use_in_place(bool flg);

    /// Sets the size at which smaller pivots are declared to be zero
    ///
    /// @param zero All pivots smaller than this will be considered zero
    void set_zero_pivot(Real zero);
};

} // namespace godzilla
