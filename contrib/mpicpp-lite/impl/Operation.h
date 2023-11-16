#pragma once

#include "mpi.h"

namespace mpicpp_lite {

namespace op {

/// Template for MPI operation `Op` on a `T` type
///
/// @tparam Op Operation
/// @tparam T Datatype
template <typename Op, typename T>
struct Operation {
};

// Sum

/// Template for summation operation on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct sum {
    /// Call operator
    ///
    /// @param x First operand
    /// @param y Second operand
    /// @return Sum of `x` and `y`
    const T &
    operator()(const T & x, const T & y) const
    {
        return x + y;
    }
};

/// Template for summation operation on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct Operation<sum<T>, T> {
    /// Call operator
    ///
    /// @return MPI operation for sumation
    static MPI_Op
    op()
    {
        return MPI_SUM;
    }
};

// Product

/// Template for product operation on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct prod {
    /// Call operator
    ///
    /// @param x First operand
    /// @param y Second operand
    /// @return Product of `x` and `y`
    const T &
    operator()(const T & x, const T & y) const
    {
        return x * y;
    }
};

/// Template for product operation on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct Operation<prod<T>, T> {
    /// Call operator
    ///
    /// @return MPI operation for product
    static MPI_Op
    op()
    {
        return MPI_PROD;
    }
};

// Maximum

/// Template for finding maximum on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct max {
    /// Call operator
    ///
    /// @param x First operand
    /// @param y Second operand
    /// @return The maximum of `x` and `y`
    const T &
    operator()(const T & x, const T & y) const
    {
        return x < y ? y : x;
    }
};

/// Template for finding maximum on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct Operation<max<T>, T> {
    /// Call operator
    ///
    /// @return MPI operation for finding maximum
    static MPI_Op
    op()
    {
        return MPI_MAX;
    }
};

// Minimum

/// Template for finding minimum on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct min {
    /// Call operator
    ///
    /// @param x First operand
    /// @param y Second operand
    /// @return The minimum of `x` and `y`
    const T &
    operator()(const T & x, const T & y) const
    {
        return x < y ? x : y;
    }
};

/// Template for finding minimum on a `T` type
///
/// @tparam T Datatype
template <typename T>
struct Operation<min<T>, T> {
    /// Call operator
    ///
    /// @return MPI operation for finding minimum
    static MPI_Op
    op()
    {
        return MPI_MIN;
    }
};

} // namespace op

} // namespace mpicpp_lite
