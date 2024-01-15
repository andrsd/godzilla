// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCFactor.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

PCFactor::PCFactor() : Preconditioner()
{
    CALL_STACK_MSG();
}

PCFactor::PCFactor(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
}

void
PCFactor::set_type(Type type)
{
    CALL_STACK_MSG();
    if (type == ICC)
        Preconditioner::set_type(PCICC);
    else if (type == ILU)
        Preconditioner::set_type(PCILU);
    else if (type == LU)
        Preconditioner::set_type(PCLU);
    else if (type == CHOLESKY)
        Preconditioner::set_type(PCCHOLESKY);
}

PCFactor::Type
PCFactor::get_type() const
{
    CALL_STACK_MSG();
    const char * name;
    PETSC_CHECK(PCGetType(this->pc, &name));
    if (strcmp(name, PCICC) == 0)
        return ICC;
    else if (strcmp(name, PCILU) == 0)
        return ILU;
    else if (strcmp(name, PCLU) == 0)
        return LU;
    else if (strcmp(name, PCCHOLESKY) == 0)
        return CHOLESKY;
    else
        throw std::logic_error("Unknown type of PCFactor preconditioner.");
}

bool
PCFactor::get_allow_diagonal_fill() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFactorGetAllowDiagonalFill(this->pc, &flg));
    return flg == PETSC_TRUE;
}

Int
PCFactor::get_levels() const
{
    CALL_STACK_MSG();
    Int levels;
    PETSC_CHECK(PCFactorGetLevels(this->pc, &levels));
    return levels;
}

PCFactor::MatSolverType
PCFactor::get_mat_solver_type() const
{
    CALL_STACK_MSG();
    ::MatSolverType stype;
    PETSC_CHECK(PCFactorGetMatSolverType(this->pc, &stype));
    if (strcmp(stype, MATSOLVERSUPERLU) == 0)
        return SUPERLU;
    else if (strcmp(stype, MATSOLVERSUPERLU_DIST) == 0)
        return SUPERLU_DIST;
    else if (strcmp(stype, MATSOLVERSTRUMPACK) == 0)
        return STRUMPACK;
    else if (strcmp(stype, MATSOLVERUMFPACK) == 0)
        return UMFPACK;
    else if (strcmp(stype, MATSOLVERCHOLMOD) == 0)
        return CHOLMOD;
    else if (strcmp(stype, MATSOLVERKLU) == 0)
        return KLU;
    else if (strcmp(stype, MATSOLVERELEMENTAL) == 0)
        return ELEMENTAL;
    else if (strcmp(stype, MATSOLVERSCALAPACK) == 0)
        return SCALAPACK;
    else if (strcmp(stype, MATSOLVERESSL) == 0)
        return ESSL;
    else if (strcmp(stype, MATSOLVERLUSOL) == 0)
        return LUSOL;
    else if (strcmp(stype, MATSOLVERMUMPS) == 0)
        return MUMPS;
    else if (strcmp(stype, MATSOLVERMKL_PARDISO) == 0)
        return MKL_PARDISO;
    else if (strcmp(stype, MATSOLVERMKL_CPARDISO) == 0)
        return MKL_CPARDISO;
    else if (strcmp(stype, MATSOLVERPASTIX) == 0)
        return PASTIX;
    else if (strcmp(stype, MATSOLVERMATLAB) == 0)
        return MATLAB;
    else if (strcmp(stype, MATSOLVERPETSC) == 0)
        return PETSC;
    else if (strcmp(stype, MATSOLVERBAS) == 0)
        return BAS;
    else if (strcmp(stype, MATSOLVERCUSPARSE) == 0)
        return CUSPARSE;
    else if (strcmp(stype, MATSOLVERCUDA) == 0)
        return CUDA;
#if PETSC_VERSION_GE(3, 18, 0)
    else if (strcmp(stype, MATSOLVERHIPSPARSE) == 0)
        return HIPSPARSE;
    else if (strcmp(stype, MATSOLVERHIP) == 0)
        return HIP;
#endif
    else if (strcmp(stype, MATSOLVERKOKKOS) == 0)
        return KOKKOS;
    else if (strcmp(stype, MATSOLVERSPQR) == 0)
        return SPQR;
    else
        throw std::logic_error("Unknown type of MatSolverType preconditioner.");
}

Real
PCFactor::get_shift_amount() const
{
    CALL_STACK_MSG();
    Real shift;
    PETSC_CHECK(PCFactorGetShiftAmount(this->pc, &shift));
    return shift;
}

PCFactor::MatShiftType
PCFactor::get_shift_type() const
{
    MatFactorShiftType type;
    PETSC_CHECK(PCFactorGetShiftType(this->pc, &type));
    return static_cast<MatShiftType>(type);
}

bool
PCFactor::get_use_in_place() const
{
    CALL_STACK_MSG();
    PetscBool flg;
    PETSC_CHECK(PCFactorGetUseInPlace(this->pc, &flg));
    return flg == PETSC_TRUE;
}

Real
PCFactor::get_zero_pivot() const
{
    CALL_STACK_MSG();
    Real pivot;
    PETSC_CHECK(PCFactorGetZeroPivot(this->pc, &pivot));
    return pivot;
}

void
PCFactor::reorder_for_nonzero_diagonal(Real tol)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorReorderForNonzeroDiagonal(this->pc, tol));
}

void
PCFactor::set_allow_diagonal_fill(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetAllowDiagonalFill(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFactor::set_column_pivot(Real dt_col)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetColumnPivot(this->pc, dt_col));
}

void
PCFactor::set_drop_tolerance(Real dt, Real dt_col, Int max_row_count)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetDropTolerance(this->pc, dt, dt_col, max_row_count));
}

void
PCFactor::set_fill(Real fill)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetFill(this->pc, fill));
}

void
PCFactor::set_levels(Int levels)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetLevels(this->pc, levels));
}

void
PCFactor::set_mat_ordering_type(MatOrderingType ordering)
{
    CALL_STACK_MSG();
    if (ordering == NATURAL)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGNATURAL));
    else if (ordering == ND)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGND));
    else if (ordering == ONE_WD)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERING1WD));
    else if (ordering == RCM)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGRCM));
    else if (ordering == QMD)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGQMD));
    else if (ordering == ROW_LENGTH)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGROWLENGTH));
    else if (ordering == WBM)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGWBM));
    else if (ordering == SPECTRAL)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGSPECTRAL));
    else if (ordering == AMD)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGAMD));
    else if (ordering == METISND)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGMETISND));
    else if (ordering == NATURAL_OR_ND)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGNATURAL_OR_ND));
    else if (ordering == EXTERNAL)
        PETSC_CHECK(PCFactorSetMatOrderingType(this->pc, MATORDERINGEXTERNAL));
}

void
PCFactor::set_mat_solver_type(MatSolverType type)
{
    CALL_STACK_MSG();
    if (type == SUPERLU)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERSUPERLU));
    else if (type == SUPERLU_DIST)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERSUPERLU_DIST));
    else if (type == STRUMPACK)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERSTRUMPACK));
    else if (type == UMFPACK)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERUMFPACK));
    else if (type == CHOLMOD)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERCHOLMOD));
    else if (type == KLU)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERKLU));
    else if (type == ELEMENTAL)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERELEMENTAL));
    else if (type == SCALAPACK)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERSCALAPACK));
    else if (type == ESSL)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERESSL));
    else if (type == LUSOL)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERLUSOL));
    else if (type == MUMPS)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERMUMPS));
    else if (type == MKL_PARDISO)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERMKL_PARDISO));
    else if (type == MKL_CPARDISO)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERMKL_CPARDISO));
    else if (type == PASTIX)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERPASTIX));
    else if (type == MATLAB)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERMATLAB));
    else if (type == PETSC)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERPETSC));
    else if (type == BAS)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERBAS));
    else if (type == CUSPARSE)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERCUSPARSE));
    else if (type == CUDA)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERCUDA));
#if PETSC_VERSION_GE(3, 18, 0)
    else if (type == HIPSPARSE)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERHIPSPARSE));
    else if (type == HIP)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERHIP));
#endif
    else if (type == KOKKOS)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERKOKKOS));
    else if (type == SPQR)
        PETSC_CHECK(PCFactorSetMatSolverType(this->pc, MATSOLVERSPQR));
}

void
PCFactor::set_pivot_in_blocks(bool pivot)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetPivotInBlocks(this->pc, pivot ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFactor::set_reuse_fill(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetReuseFill(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFactor::set_reuse_ordering(bool flag)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetReuseOrdering(this->pc, flag ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFactor::set_shift_amount(Real shift_amount)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetShiftAmount(this->pc, shift_amount));
}

void
PCFactor::set_shift_type(MatShiftType type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetShiftType(this->pc, static_cast<MatFactorShiftType>(type)));
}

void
PCFactor::set_up_mat_solver_type()
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetUpMatSolverType(this->pc));
}

void
PCFactor::set_use_in_place(bool flg)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetUseInPlace(this->pc, flg ? PETSC_TRUE : PETSC_FALSE));
}

void
PCFactor::set_zero_pivot(Real zero)
{
    CALL_STACK_MSG();
    PETSC_CHECK(PCFactorSetZeroPivot(this->pc, zero));
}

} // namespace godzilla
