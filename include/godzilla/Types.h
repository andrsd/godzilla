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

/// This is the API that we hand to PETSc for fields.
///
/// @param dim The spatial dimension
/// @param Nf The number of input fields
/// @param NfAux The number of input auxiliary fields
/// @param uOff The offset of each field in u[]
/// @param uOff_x The offset of each field in u_x[]
/// @param u The field values at this point in space
/// @param u_t The field time derivative at this point in space (or `nullptr`)
/// @param u_x The field derivatives at this point in space
/// @param aOff The offset of each auxiliary field in u[]
/// @param aOff_x The offset of each auxiliary field in u_x[]
/// @param a The auxiliary field values at this point in space
/// @param a_t The auxiliary field time derivative at this point in space (or `nullptr`)
/// @param a_x The auxiliary field derivatives at this point in space
/// @param t The current time
/// @param x The coordinates of this point
/// @param numConstants The number of constants
/// @param constants The value of each constant
/// @param f The value of the function at this point in space
typedef void PetscFieldFunc(Int dim,
                            Int Nf,
                            Int NfAux,
                            const Int uOff[],
                            const Int uOff_x[],
                            const Scalar u[],
                            const Scalar u_t[],
                            const Scalar u_x[],
                            const Int aOff[],
                            const Int aOff_x[],
                            const Scalar a[],
                            const Scalar a_t[],
                            const Scalar a_x[],
                            Real t,
                            const Real x[],
                            Int numConstants,
                            const Scalar constants[],
                            Scalar f[]);

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
