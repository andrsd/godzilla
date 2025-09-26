// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Enums.h"
#include "godzilla/Flags.h"
#include "godzilla/Delegate.h"
#include "petscsystypes.h"

namespace godzilla {

using Int = PetscInt;
using Real = PetscReal;
using Scalar = PetscScalar;
using ErrorCode = PetscErrorCode;

typedef ErrorCode PetscFunc(Int dim, Real time, const Real x[], Int Nc, Scalar u[], void * ctx);

using FunctionDelegate = Delegate<void(Real, const Real[], Scalar[])>;

namespace internal {

ErrorCode
invoke_function_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx);

} // namespace internal

//

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

using ExecuteOn = Flags<ExecuteOnFlag>;

/// Dimension
class Dimension {
public:
    enum Value {
        // Invalid
        INVALID = -1,
        ZERO = 0,
        ONE = 1,
        TWO = 2,
        THREE = 3
    };

    constexpr Dimension() : value(INVALID) {}
    constexpr explicit Dimension(Value v) : value(v) {}

    constexpr bool
    is_valid() const
    {
        return this->value != INVALID;
    }

    // Comparison operators
    constexpr bool
    operator==(const Dimension & other) const
    {
        return this->value == other.value;
    }

    constexpr bool
    operator!=(const Dimension & other) const
    {
        return !(*this == other);
    }

    constexpr
    operator Int() const
    {
        return this->value;
    }

    Value value;

    static constexpr Dimension
    from_int(Int v)
    {
        switch (v) {
        case 0:
            return Dimension(ZERO);
        case 1:
            return Dimension(ONE);
        case 2:
            return Dimension(TWO);
        case 3:
            return Dimension(THREE);
        default:
            return Dimension(INVALID);
        }
    }
};

// User-defined literal
constexpr Dimension
operator""_D(unsigned long long n)
{
    switch (n) {
    case 0:
        return Dimension(Dimension::ZERO);
    case 1:
        return Dimension(Dimension::ONE);
    case 2:
        return Dimension(Dimension::TWO);
    case 3:
        return Dimension(Dimension::THREE);
    default:
        return Dimension();
    }
}

/// Order
class Order {
public:
    explicit constexpr Order(Int order) : num(order) {}

    constexpr Int
    value() const
    {
        return num;
    }

    constexpr bool
    operator==(Int other) const
    {
        return this->num == other;
    }

    constexpr bool
    operator!=(Int other) const
    {
        return this->num != other;
    }

private:
    Int num;
};

/// FieldID
class FieldID {
public:
    explicit constexpr FieldID(Int fid) : num(fid) {}
    ~FieldID() = default;
    constexpr FieldID(const FieldID &) = default;
    constexpr FieldID(FieldID &&) noexcept = default;
    constexpr FieldID & operator=(const FieldID &) = default;
    constexpr FieldID & operator=(FieldID &&) noexcept = default;

    constexpr Int
    value() const
    {
        return this->num;
    }

    constexpr bool
    operator<(FieldID other) const
    {
        return this->num < other.num;
    }

    constexpr bool
    operator==(FieldID other) const
    {
        return this->num == other.num;
    }

    constexpr bool
    operator!=(FieldID other) const
    {
        return this->num != other.num;
    }

    static const FieldID INVALID;

private:
    Int num;
};

inline constexpr FieldID FieldID::INVALID { -1 };

namespace fe {

/// Primary template for a simplex element type selector
template <Dimension D>
class SimplexSelector;

template <>
class SimplexSelector<1_D> {
public:
    static constexpr ElementType ELEMENT_TYPE = EDGE2;
    static constexpr Int N_NODES = 2;
    static constexpr Int N_FACE_NODES = 1;
};

template <>
class SimplexSelector<2_D> {
public:
    static constexpr ElementType ELEMENT_TYPE = TRI3;
    static constexpr Int N_NODES = 3;
    static constexpr Int N_FACE_NODES = 2;
};

template <>
class SimplexSelector<3_D> {
public:
    static constexpr ElementType ELEMENT_TYPE = TET4;
    static constexpr Int N_NODES = 4;
    static constexpr Int N_FACE_NODES = 3;
};

template <Dimension D>
constexpr Int N_NODES = SimplexSelector<D>::N_NODES;

} // namespace fe

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

} // namespace godzilla
