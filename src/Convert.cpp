// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Convert.h"
#include "godzilla/Enums.h"
#include "godzilla/CallStack.h"
#include "godzilla/KrylovSolver.h"

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

} // namespace conv
} // namespace godzilla
