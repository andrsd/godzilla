// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCFieldSplit.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

PCFieldSplit::PCFieldSplit() : Preconditioner()
{
    CALL_STACK_MSG();
}

PCFieldSplit::PCFieldSplit(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
    Preconditioner::set_type(PCFIELDSPLIT);
}

void
PCFieldSplit::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
    Preconditioner::create(comm);
    Preconditioner::set_type(PCFIELDSPLIT);
}

void
PCFieldSplit::set_type(Type type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetType(this->pc, static_cast<PCCompositeType>(type)));
}

PCFieldSplit::Type
PCFieldSplit::get_type() const
{
    CALL_STACK_MSG();
    PCCompositeType type;
    PETSC_CHECK(PCFieldSplitGetType(this->pc, &type));
    return static_cast<Type>(type);
}

bool
PCFieldSplit::get_dm_splits() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFieldSplitGetDMSplits(this->pc, &flg));
    return flg == PETSC_TRUE;
}

bool
PCFieldSplit::get_detect_saddle_point() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFieldSplitGetDetectSaddlePoint(this->pc, &flg));
    return flg == PETSC_TRUE;
}

bool
PCFieldSplit::get_diag_use_amat() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFieldSplitGetDiagUseAmat(this->pc, &flg));
    return flg == PETSC_TRUE;
}

IndexSet
PCFieldSplit::get_is(const std::string & split_name) const
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(PCFieldSplitGetIS(this->pc, split_name.c_str(), &is));
    return IndexSet(is);
}

IndexSet
PCFieldSplit::get_is_by_index(Int index) const
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(PCFieldSplitGetISByIndex(this->pc, index, &is));
    return IndexSet(is);
}

bool
PCFieldSplit::get_off_diag_use_amat() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFieldSplitGetOffDiagUseAmat(this->pc, &flg));
    return flg == PETSC_TRUE;
}

PCFieldSplit::SchurBlocks
PCFieldSplit::get_schur_blocks() const
{
    CALL_STACK_MSG();
    Mat A00, A01, A10, A11;
    PETSC_CHECK(PCFieldSplitGetSchurBlocks(this->pc, &A00, &A01, &A10, &A11));
    SchurBlocks blks(A00, A01, A10, A11);
    return blks;
}

PCFieldSplit::SchurPC
PCFieldSplit::get_schur_pre() const
{
    CALL_STACK_MSG();
    PCFieldSplitSchurPreType ptype;
    Mat pre;
    PETSC_CHECK(PCFieldSplitGetSchurPre(this->pc, &ptype, &pre));
    SchurPC spc(ptype, pre);
    return spc;
}

std::vector<KrylovSolver>
PCFieldSplit::get_sub_ksp() const
{
    CALL_STACK_MSG();
    PetscInt n;
    KSP * subksp;
    PETSC_CHECK(PCFieldSplitGetSubKSP(this->pc, &n, &subksp));
    std::vector<KrylovSolver> sks(n);
    for (Int i = 0; i < n; i++)
        sks[i] = KrylovSolver(subksp[i]);
    PetscFree(subksp);
    return sks;
}

Matrix
PCFieldSplit::schur_get_s() const
{
    CALL_STACK_MSG();
    Mat s;
    PETSC_CHECK(PCFieldSplitSchurGetS(this->pc, &s));
    return Matrix(s);
}

std::vector<KrylovSolver>
PCFieldSplit::schur_get_sub_ksp() const
{
    CALL_STACK_MSG();
    PetscInt n;
    KSP * subksp;
    PETSC_CHECK(PCFieldSplitSchurGetSubKSP(this->pc, &n, &subksp));
    std::vector<KrylovSolver> sks(n);
    for (Int i = 0; i < n; i++)
        sks[i] = KrylovSolver(subksp[i]);
    PetscFree(subksp);
    return sks;
}

void
PCFieldSplit::schur_restore_s(const Matrix & s)
{
    CALL_STACK_MSG();
    Mat m = s;
    PETSC_CHECK(PCFieldSplitSchurRestoreS(this->pc, &m));
}

void
PCFieldSplit::set_block_size(Int bs)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetBlockSize(this->pc, bs));
}

void
PCFieldSplit::set_dm_splits(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetDMSplits(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFieldSplit::set_detect_saddle_point(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetDetectSaddlePoint(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFieldSplit::set_diag_use_amat(bool flg)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetDiagUseAmat(this->pc, flg ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFieldSplit::set_fields(const std::string & split_name,
                         const std::vector<Int> & fields,
                         const std::vector<Int> & fields_col)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetFields(this->pc,
                                      split_name.c_str(),
                                      fields.size(),
                                      fields.data(),
                                      fields_col.data()));
}

void
PCFieldSplit::set_gkb_delay(Int delay)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetGKBDelay(this->pc, delay));
}

void
PCFieldSplit::set_gkb_maxit(Int maxit)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetGKBMaxit(this->pc, maxit));
}

void
PCFieldSplit::set_gkb_nu(Real nu)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetGKBNu(this->pc, nu));
}

void
PCFieldSplit::set_gkb_tol(Real tolerance)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetGKBTol(this->pc, tolerance));
}

void
PCFieldSplit::set_is(const std::string & split_name, const IndexSet & is)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetIS(this->pc, split_name.c_str(), is));
}

void
PCFieldSplit::set_off_diag_use_amat(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetOffDiagUseAmat(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFieldSplit::set_schur_fact_type(PCFieldSplit::SchurFactType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(
        PCFieldSplitSetSchurFactType(this->pc, static_cast<PCFieldSplitSchurFactType>(type)));
}

void
PCFieldSplit::set_schur_pre(PCFieldSplit::SchurPreType ptype, const Matrix & pre)
{
    CALL_STACK_MSG();
    PETSC_CHECK(
        PCFieldSplitSetSchurPre(this->pc, static_cast<PCFieldSplitSchurPreType>(ptype), pre));
}

void
PCFieldSplit::set_schur_scale(Scalar scale)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFieldSplitSetSchurScale(this->pc, scale));
}

} // namespace godzilla
