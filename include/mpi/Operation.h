#pragma once

#include "mpi.h"

namespace godzilla {

namespace mpi {

namespace op {

template <typename Op, typename T>
struct Operation {
};

// Sum

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

template <typename T>
struct Operation<sum<T>, T> {
    static MPI_Op
    op()
    {
        return MPI_SUM;
    }
};

// Product

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

template <typename T>
struct Operation<prod<T>, T> {
    static MPI_Op
    op()
    {
        return MPI_PROD;
    }
};

// Maximum

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

template <typename T>
struct Operation<max<T>, T> {
    static MPI_Op
    op()
    {
        return MPI_MAX;
    }
};

// Minimum

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

template <typename T>
struct Operation<min<T>, T> {
    static MPI_Op
    op()
    {
        return MPI_MIN;
    }
};

} // namespace op

} // namespace mpi

} // namespace godzilla
