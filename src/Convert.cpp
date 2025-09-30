// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Convert.h"
#include "godzilla/Enums.h"
#include "godzilla/CallStack.h"
#include "godzilla/KrylovSolver.h"
#include "godzilla/SNESolver.h"
#include "godzilla/Exception.h"

namespace godzilla {
namespace conv {

template <>
std::string
to_str(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    switch (elem_type) {
    case PolytopeType::POINT:
        return "POINT";
    case PolytopeType::SEGMENT:
        return "SEGMENT";
    case PolytopeType::POINT_PRISM_TENSOR:
        return "POINT_PRISM_TENSOR";
    case PolytopeType::TRIANGLE:
        return "TRIANGLE";
    case PolytopeType::QUADRILATERAL:
        return "QUADRILATERAL";
    case PolytopeType::SEG_PRISM_TENSOR:
        return "SEG_PRISM_TENSOR";
    case PolytopeType::TETRAHEDRON:
        return "TETRAHEDRON";
    case PolytopeType::HEXAHEDRON:
        return "HEXAHEDRON";
    case PolytopeType::TRI_PRISM:
        return "TRI_PRISM";
    case PolytopeType::TRI_PRISM_TENSOR:
        return "TRI_PRISM_TENSOR";
    case PolytopeType::QUAD_PRISM_TENSOR:
        return "QUAD_PRISM_TENSOR";
    case PolytopeType::PYRAMID:
        return "PYRAMID";
    case PolytopeType::FV_GHOST:
        return "FV_GHOST";
    case PolytopeType::INTERIOR_GHOST:
        return "INTERIOR_GHOST";
    default:
        return "UNKNOWN";
    }
}

template <>
std::string
to_str(const ElementType type)
{
    switch (type) {
    case EDGE2:
        return "EDGE2";
    case TRI3:
        return "TRI3";
    case QUAD4:
        return "QUAD4";
    case TET4:
        return "TET4";
    case HEX8:
        return "HEX8";
    default:
        throw InternalError("Unsupported element type");
    }
}

template <>
std::string
to_str(KrylovSolver::ConvergedReason reason)
{
    CALL_STACK_MSG();
    if (reason == KrylovSolver::ConvergedReason::CONVERGED_ITERATING)
        return "iterating";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_RTOL_NORMAL)
        return "relative tolerance";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_ATOL_NORMAL)
        return "absolute tolerance";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_RTOL)
        return "relative tolerance";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_ATOL)
        return "absolute tolerance";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_ITS)
        return "maximum iterations";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_STEP_LENGTH)
        return "step length";
    else if (reason == KrylovSolver::ConvergedReason::CONVERGED_HAPPY_BREAKDOWN)
        return "happy breakdown";
    // Diverged reasons
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_NULL)
        return "null";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_ITS)
        return "maximum iterations";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_DTOL)
        return "divergent tolerance";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_BREAKDOWN)
        return "breakdown";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_BREAKDOWN_BICG)
        return "breakdown (BiCG)";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_NONSYMMETRIC)
        return "non-symmetric matrix";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_INDEFINITE_PC)
        return "indefinite preconditioner";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_NANORINF)
        return "NaN or inf values";
    else if (reason == KrylovSolver::ConvergedReason::DIVERGED_INDEFINITE_MAT)
        return "indefinite matrix";
    else
        return "unknown";
}

template <>
std::string
to_str(SNESolver::ConvergedReason reason)
{
    CALL_STACK_MSG();
    if (reason == SNESolver::ConvergedReason::CONVERGED_ITERATING)
        return "iterating";
    else if (reason == SNESolver::ConvergedReason::CONVERGED_FNORM_ABS)
        return "absolute function norm";
    else if (reason == SNESolver::ConvergedReason::CONVERGED_FNORM_RELATIVE)
        return "relative function norm";
    else if (reason == SNESolver::ConvergedReason::CONVERGED_SNORM_RELATIVE)
        return "relative step norm";
    else if (reason == SNESolver::ConvergedReason::CONVERGED_ITS)
        return "maximum iterations";
    else if (reason == SNESolver::ConvergedReason::BREAKOUT_INNER_ITER)
        return "inner iteration breakout";
    // Diverged reasons
    else if (reason == SNESolver::ConvergedReason::DIVERGED_FUNCTION_DOMAIN)
        return "function domain";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_FUNCTION_COUNT)
        return "function count";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_LINEAR_SOLVE)
        return "diverged linear solve";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_FNORM_NAN)
        return "function norm in NaN";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_MAX_IT)
        return "maximum iterations";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_LINE_SEARCH)
        return "diverged line search";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_INNER)
        return "inner solve failed";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_LOCAL_MIN)
        return "local minimum";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_DTOL)
        return "divergence tolerance";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_JACOBIAN_DOMAIN)
        return "Jacobian domain";
    else if (reason == SNESolver::ConvergedReason::DIVERGED_TR_DELTA)
        return "trust region delta";
    else
        return "unknown";
}

template <>
std::string
to_str(SNESolver::LineSearch::LineSearchType type)
{
    CALL_STACK_MSG();
    if (type == SNESolver::LineSearch::BASIC)
        return "basic";
    else if (type == SNESolver::LineSearch::L2)
        return "l2";
    else if (type == SNESolver::LineSearch::CP)
        return "cp";
    else if (type == SNESolver::LineSearch::NLEQERR)
        return "nleqerr";
    else if (type == SNESolver::LineSearch::SHELL)
        return "shell";
    else if (type == SNESolver::LineSearch::BT)
        return "bt";
    else
        return "unknown";
}

} // namespace conv
} // namespace godzilla
