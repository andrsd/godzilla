// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscsys.h"
#include "petscdmtypes.h"

namespace godzilla {

enum CopyMode {
    /// The array values are copied into new space, the user is free to reuse or delete the passed
    /// in array
    COPY_VALUES = PETSC_COPY_VALUES,
    ///
    OWN_POINTER = PETSC_OWN_POINTER,
    /// The array values are *not* copied, the object uses the array but does *not* take ownership
    /// of the array. The user cannot use the array but the user must delete the array after the
    /// object is destroyed.
    USE_POINTER = PETSC_USE_POINTER,
};

enum class FileAccess {
    /// Open the file for reading
    READ,
    /// Open the file for writing
    WRITE,
    /// Create a new file
    CREATE
};

/// Coordinate system type
enum CoordinateType {
    /// Cartesian coordinates
    CARTESIAN,
    /// Spherical coordinates
    SPHERICAL,
    /// Cylindrical coordinates
    AXISYMMETRIC
};

/// Element tyoe
enum ElementType {
    /// 2-node edge element
    EDGE2,
    /// 3-node triangle element
    TRI3,
    /// 4-node quadrilateral element
    QUAD4,
    /// 4-node tetrahedral element
    TET4,
    /// 8-node hexahedral element
    HEX8
};

enum ExecuteOnFlag : unsigned int {
    EXECUTE_ON_INITIAL = 0x1,
    EXECUTE_ON_TIMESTEP = 0x2,
    EXECUTE_ON_FINAL = 0x4
};

enum class PolytopeType {
    POINT = DM_POLYTOPE_POINT,
    SEGMENT = DM_POLYTOPE_SEGMENT,
    POINT_PRISM_TENSOR = DM_POLYTOPE_POINT_PRISM_TENSOR,
    TRIANGLE = DM_POLYTOPE_TRIANGLE,
    QUADRILATERAL = DM_POLYTOPE_QUADRILATERAL,
    SEG_PRISM_TENSOR = DM_POLYTOPE_SEG_PRISM_TENSOR,
    TETRAHEDRON = DM_POLYTOPE_TETRAHEDRON,
    HEXAHEDRON = DM_POLYTOPE_HEXAHEDRON,
    PYRAMID = DM_POLYTOPE_PYRAMID,
    TRI_PRISM = DM_POLYTOPE_TRI_PRISM,
    TRI_PRISM_TENSOR = DM_POLYTOPE_TRI_PRISM_TENSOR,
    QUAD_PRISM_TENSOR = DM_POLYTOPE_QUAD_PRISM_TENSOR,
    FV_GHOST = DM_POLYTOPE_FV_GHOST,
    INTERIOR_GHOST = DM_POLYTOPE_INTERIOR_GHOST,
    UNKNOWN = DM_POLYTOPE_UNKNOWN
};

} // namespace godzilla
