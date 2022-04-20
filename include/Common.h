#pragma once

#include "GodzillaConfig.h"
#include <Judy.h>

namespace godzilla {

// 1D element modes
enum EMode1D { MODE_EDGE = 0 };

// 2D element modes
enum EMode2D { MODE_TRIANGLE = 0, MODE_QUAD = 1 };

// 3D element modes
enum EMode3D { MODE_TETRAHEDRON = 0, MODE_HEXAHEDRON = 1, MODE_PRISM = 2 };

/// number of types of values in EValueType
static const unsigned int NUM_VALUE_TYPES = 10;

enum EValueType {
    FN = 0,
    DX = 1,
    DY = 2,
    DZ = 3,
    DXX = 4,
    DYY = 5,
    DZZ = 6,
    DXY = 7,
    DYZ = 8,
    DXZ = 9
};

/// 1D point
struct Point1D {
    /// coordinates of a point
    Real x;
};

/// 2D point
struct Point2D {
    /// coordinates of a point
    Real x, y;
};

/// 3D point
struct Point3D {
    /// coordinates of a point
    Real x, y, z;
};

// Convenience types

/// For all indexing
typedef Word_t Index;

const Index INVALID_IDX = (Index) -1;

typedef unsigned int uint;

typedef Real Real2[2];
typedef Real Real3[3];
typedef Real Real2x2[2][2];
typedef Real Real3x3[3][3];
typedef int int2[2];
typedef uint uint2[2];

} // namespace godzilla
