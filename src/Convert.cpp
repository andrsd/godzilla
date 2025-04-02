// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Convert.h"
#include "godzilla/Enums.h"
#include "godzilla/CallStack.h"

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

} // namespace conv
} // namespace godzilla
