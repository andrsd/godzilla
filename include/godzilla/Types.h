// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petsc.h"
#include "godzilla/Flags.h"

namespace godzilla {

typedef PetscInt Int;
typedef PetscReal Real;
typedef PetscScalar Scalar;
typedef PetscErrorCode ErrorCode;

typedef ErrorCode PetscFunc(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx);

//

enum CoordinateType { CARTESIAN, SPHERICAL, AXISYMMETRIC };

enum ElementType { EDGE2, TRI3, QUAD4, TET4, HEX8 };

/// Return the text representation of an element type
///
/// @param type Element type
/// @return Text representation of an element type
std::string get_element_type_str(const ElementType & type);

/// Return number of nodes given FE type
///
/// @param type Element type
/// @return Number of nodes per element
constexpr Int
get_num_element_nodes(ElementType type)
{
    switch (type) {
    case EDGE2:
        return 2;
    case TRI3:
        return 3;
    case QUAD4:
        return 4;
    case TET4:
        return 4;
    case HEX8:
        return 8;
    }
}

enum ExecuteOnFlag : unsigned int {
    EXECUTE_ON_INITIAL = 0x1,
    EXECUTE_ON_TIMESTEP = 0x2,
    EXECUTE_ON_FINAL = 0x4
};

using ExecuteOn = Flags<ExecuteOnFlag>;

} // namespace godzilla

namespace std {

template <>
struct less<PetscFormKey> {
    bool
    operator()(const PetscFormKey & lhs, const PetscFormKey & rhs) const
    {
        if (lhs.label == rhs.label) {
            if (lhs.value == rhs.value) {
                if (lhs.field == rhs.field) {
                    return lhs.part < rhs.part;
                }
                else
                    return lhs.field < rhs.field;
            }
            else
                return lhs.value < rhs.value;
        }
        else
            return lhs.label < rhs.label;
    }
};

} // namespace std
