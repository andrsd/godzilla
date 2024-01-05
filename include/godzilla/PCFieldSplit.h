// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Matrix.h"
#include "godzilla/KrylovSolver.h"

namespace godzilla {

class PCFieldSplit : public Preconditioner {
public:
    enum Type {
        ADDITIVE = PC_COMPOSITE_ADDITIVE,
        COMPOSITE_MULTIPLICATIVE = PC_COMPOSITE_MULTIPLICATIVE,
        SYMMETRIC_MULTIPLICATIVE = PC_COMPOSITE_SYMMETRIC_MULTIPLICATIVE,
        SPECIAL = PC_COMPOSITE_SPECIAL,
        SCHUR = PC_COMPOSITE_SCHUR,
        GBK = PC_COMPOSITE_GKB
    };

    enum SchurFactType {
        SCHUR_FACT_DIAG = PC_FIELDSPLIT_SCHUR_FACT_DIAG,
        SCHUR_FACT_LOWER = PC_FIELDSPLIT_SCHUR_FACT_LOWER,
        SCHUR_FACT_UPPER = PC_FIELDSPLIT_SCHUR_FACT_UPPER,
        SCHUR_FACT_FULL = PC_FIELDSPLIT_SCHUR_FACT_FULL
    };

    enum SchurPreType {
        SCHUR_PRE_SELF = PC_FIELDSPLIT_SCHUR_PRE_SELF,
        SCHUR_PRE_SELFP = PC_FIELDSPLIT_SCHUR_PRE_SELFP,
        SCHUR_PRE_A11 = PC_FIELDSPLIT_SCHUR_PRE_A11,
        SCHUR_PRE_USER = PC_FIELDSPLIT_SCHUR_PRE_USER,
        SCHUR_PRE_FULL = PC_FIELDSPLIT_SCHUR_PRE_FULL
    };

    struct SchurBlocks {
        SchurBlocks(Mat A00, Mat A01, Mat A10, Mat A11) : A00(A00), A01(A01), A10(A10), A11(A11) {}

        Matrix A00;
        Matrix A01;
        Matrix A10;
        Matrix A11;
    };

    struct SchurPC {
        SchurPC(PCFieldSplitSchurPreType type, Mat pre) :
            type(static_cast<SchurPreType>(type)),
            matrix(pre)
        {
        }

        SchurPreType type;
        Matrix matrix;
    };

    PCFieldSplit();
    PCFieldSplit(PC pc);

    /// Creates a preconditioner
    ///
    /// @param comm MPI communicator
    void create(MPI_Comm comm);

    /// Sets the type of the field split preconditioner
    ///
    /// @param type Type of the preconditioner
    void set_type(Type type);

    /// Gets the type of the field split preconditioner
    ///
    /// @return Type of the preconditioner
    Type get_type() const;

    /// Returns flag indicating whether `DMCreateFieldDecomposition()` should be used to define the
    /// splits, whenever possible.
    bool get_dm_splits() const;

    /// Returns flag indicating whether this preconditioner will attempt to automatically determine
    /// fields based on zero diagonal entries.
    bool get_detect_saddle_point() const;

    /// Get the flag indicating whether to extract diagonal blocks from `Amat` (rather than `Pmat`)
    /// to build the sub-matrices associated with each split. Where
    /// `KSPSetOperators(ksp, Amat, Pmat)` was used to supply the operators.
    bool get_diag_use_amat() const;

    /// Retrieves the elements for a split as an IndexSet
    ///
    /// @param split_name Name of the split
    /// @return The `IndexSet` that defines the elements in this split
    IndexSet get_is(const std::string & split_name) const;

    /// Retrieves the elements for a given split as an IS
    ///
    /// @param index Index of the split
    /// @return The index set that defines the elements in this split
    IndexSet get_is_by_index(Int index) const;

    /// Get the flag indicating whether to extract off-diagonal blocks from `Amat` (rather than
    /// `Pmat`) to build the sub-matrices associated with each split. Where
    /// `KSPSetOperators(ksp, Amat, Pmat)` was used to supply the operators.
    bool get_off_diag_use_amat() const;

    /// Gets all matrix blocks for the Schur complement
    SchurBlocks get_schur_blocks() const;

    /// For Schur complement fieldsplit, determine how the Schur complement will be preconditioned.
    SchurPC get_schur_pre() const;

    /// Gets the `KrylovSolver`s for all splits
    std::vector<KrylovSolver> get_sub_ksp() const;

    /// Extract the MATSCHURCOMPLEMENT object used by this PCFIELDSPLIT in case it needs to be
    /// configured separately
    ///
    /// @return The Schur complement matrix
    Matrix schur_get_s() const;

    /// Gets the KSP contexts used inside the Schur complement based PCFIELDSPLIT
    ///
    /// @return The array of `KrylovSolver`s
    std::vector<KrylovSolver> schur_get_sub_ksp() const;

    /// Restore the `MATSCHURCOMPLEMENT` matrix used by this preconditioner
    ///
    /// @param s The Schur complement matrix
    void schur_restore_s(const Matrix & s);

    /// Sets the block size for defining where fields start in the field split preconditioner when
    /// calling `set_is()`. If not set the matrix block size is used.
    ///
    /// @param bs The block size
    void set_block_size(Int bs);

    /// Flags whether `DMCreateFieldDecomposition()` should be used to define the splits in a
    /// `PCFIELDSPLIT`, whenever possible.
    ///
    /// @param flag Boolean indicating whether to use field splits defined by the DM
    void set_dm_splits(bool flag);

    /// Sets flag indicating whether PCFIELDSPLIT will attempt to automatically determine fields
    /// based on zero diagonal entries.
    ///
    /// @param flag Boolean indicating whether to detect fields or not
    void set_detect_saddle_point(bool flag);

    /// Set flag indicating whether to extract diagonal blocks from `Amat` (rather than `Pmat`) to
    /// build the sub-matrices associated with each split. Where `KSPSetOperators(ksp, Amat, Pmat)`
    /// was used to supply the operators.
    ///
    /// @param flg Boolean flag indicating whether or not to use Amat to extract the diagonal blocks
    /// from
    void set_diag_use_amat(bool flg);

    /// Sets the fields that define one particular split in PCFIELDSPLIT
    ///
    /// @param split_name Name of this split, if empty the number of the split is used
    /// @param fields The fields in this split
    /// @param fields_col Generally the same as `fields`, if it does not match `fields` then the
    /// matrix block that is solved for this set of fields comes from an off-diagonal block of the
    /// matrix and `fields_col` provides the column indices for that block
    void set_fields(const std::string & split_name,
                    const std::vector<Int> & fields,
                    const std::vector<Int> & fields_col);

    /// Sets the delay in the lower bound error estimate in the generalized Golub-Kahan
    /// bidiagonalization [Ari13] in PCFIELDSPLIT preconditioner.
    ///
    /// @param delay The delay window in the lower bound estimate
    void set_gkb_delay(Int delay);

    /// Sets the maximum number of iterations for the generalized Golub-Kahan bidiagonalization
    /// preconditioner in PCFIELDSPLIT
    void set_gkb_maxit(Int maxit);

    /// Sets the scalar value nu >= 0 in the transformation H = A00 + nu A01 A01’ of the (1,1) block
    /// in the Golub-Kahan bidiagonalization preconditioner [Ari13] in PCFIELDSPLIT
    ///
    /// @param nu The shift parameter
    void set_gkb_nu(Real nu);

    /// Sets the solver tolerance for the generalized Golub-Kahan bidiagonalization preconditioner
    /// [Ari13] in PCFIELDSPLIT
    ///
    /// @param tolerance The solver tolerance
    void set_gkb_tol(Real tolerance);

    /// Sets the exact elements for a split in a PCFIELDSPLIT
    ///
    /// @param split_name Name of this split, if empty the number of the split is used
    /// @param is The `IndexSet` that defines the elements in this split
    void set_is(const std::string & split_name, const IndexSet & is);

    /// Set flag indicating whether to extract off-diagonal blocks from `Amat` (rather than `Pmat`)
    /// to build the sub-matrices associated with each split. Where
    /// `KSPSetOperators(ksp, Amat, Pmat)` was used to supply the operators.
    ///
    /// @param flag Boolean flag indicating whether or not to use `Amat` to extract the off-diagonal
    /// blocks from
    void set_off_diag_use_amat(bool flag);

    /// Sets which blocks of the approximate block factorization to retain in the preconditioner
    /// [MGW00] and [Ips01]
    ///
    /// @param type Which blocks of factorization to retain
    void set_schur_fact_type(SchurFactType type);

    /// Indicates from what operator the preconditioner is constructed for the Schur complement.
    /// The default is the A11 matrix.
    ///
    /// @param ptype Which matrix to use for preconditioning the Schur complement
    /// @param pre Matrix to use for preconditioning
    void set_schur_pre(SchurPreType ptype, const Matrix & pre);

    /// Controls the sign flip of S for `SCHUR_FACT_DIAG`.
    ///
    /// @param scale Scaling factor for the Schur complement
    void set_schur_scale(Scalar scale);
};

} // namespace godzilla
