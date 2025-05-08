// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/IndexSet.h"
#include "godzilla/Types.h"
#include "godzilla/DenseVector.h"
#include "mpi.h"
#include "petscvec.h"
#include <vector>

namespace godzilla {

class Vector {
public:
    Vector();
    Vector(MPI_Comm comm);
    Vector(Vec vec);

    void create(MPI_Comm comm);
    void destroy();
    void set_name(const std::string & name);

    void set_up();

    /// Convenience function that does `assembly_begin()` and `assembly_end()`
    void assemble();
    void assembly_begin();
    void assembly_end();

    /// Get the vector type name
    ///
    /// @return The vector type
    std::string get_type() const;

    Scalar * get_array();
    const Scalar * get_array_read() const;
    void restore_array(Scalar * arr);
    void restore_array_read(const Scalar * arr) const;

    /// Returns the global number of elements of the vector
    Int get_size() const;
    /// Returns the local number of elements of the vector
    Int get_local_size() const;
    void get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const;

    void abs();
    [[deprecated("")]] Scalar dot(const Vector & y) const;
    void scale(Scalar alpha);

    [[deprecated("")]] void duplicate(Vector & b) const;
    Vector duplicate() const;

    /// Copy this vector into `y`
    ///
    /// @param y Vector to copy our values into
    [[deprecated("Message")]] void copy(Vector & y) const;

    /// Copies between a reduced vector and the appropriate elements of a full-space vector.
    ///
    /// @param is Index set for the reduced space
    /// @param mode Direction of copying (SCATTER_FORWARD or SCATTER_REVERSE)
    /// @param reduced Reduced-space vector
    void copy(const IndexSet & is, ScatterMode mode, Vector & reduced);

    /// Assign values of y into this vector
    ///
    /// @param y Vector with values to be assined into this vector
    void assign(const Vector & y);

    /// Computes the vector norm
    ///
    /// @param type The type of the norm requested
    /// @return The norm
    Real norm(NormType type) const;

    void normalize();
    void shift(Scalar shift);
    Scalar min() const;
    Scalar max() const;
#if PETSC_VERSION_LT(3, 20, 0)
    void chop(Real tol);
#else
    [[deprecated("Use filter() instead")]] void chop(Real tol);
#endif

#if PETSC_VERSION_GE(3, 20, 0)
    /// Set all values in the vector with an absolute value less than or equal to the tolerance to
    /// zero
    ///
    /// @param tol The zero tolerance
    void filter(Real tol);
#endif

    /// Computes `this[i] = alpha x[i] + this[i]`
    ///
    /// @param alpha Scalar
    /// @param x Vector to scale by `alpha`
    [[deprecated("")]] void axpy(Scalar alpha, const Vector & x);

    /// Computes `this[i] = alpha x[i] + beta y[i]`
    ///
    /// @param alpha Scalar
    /// @param beta Scalar
    /// @param x Vector sclaed by `alpha`
    [[deprecated("")]] void axpby(Scalar alpha, Scalar beta, const Vector & x);

    /// Computes `this[i] = x[i] + beta * this[i]`
    ///
    /// @param beta Scalar
    /// @param x Unscaled vector
    [[deprecated("")]] void aypx(Scalar beta, const Vector & x);

    /// Computes `this[i] = alpha * x[i] + y[i]
    ///
    /// @param alpha Scalar
    /// @param x First vector multiplied by `alpha`
    /// @param y Second vector
    [[deprecated("")]] void waxpy(Scalar alpha, const Vector & x, const Vector & y);

    /// Computes `this = this + \sum_i alpha[i] x[i]`
    ///
    /// Length of `alpha` and `x` must be the same
    ///
    /// @param alpha Array of scalars
    /// @param x Array of vectors
    [[deprecated("")]] void maxpy(const std::vector<Scalar> & alpha, const std::vector<Vector> & x);

    /// Computes `this = alpha * x + beta * y + gamma * this`
    ///
    /// @param alpha First scalar
    /// @param beta Second scalar
    /// @param gamma Third scalar
    /// @param x First vector
    /// @param y Second vector
    [[deprecated("")]] void
    axpbypcz(Scalar alpha, Scalar beta, Scalar gamma, const Vector & x, const Vec & y);

    void reciprocal();

    void set(Scalar alpha);
    void set_sizes(Int n, Int N = PETSC_DECIDE);
    void set_block_size(Int bs);
    void set_type(VecType method);

    void set_value(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_value_local(Int row, Scalar value, InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector
    ///
    /// @param n Number of elements to insert
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    void set_values(Int n, const Int * ix, const Scalar * y, InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector
    ///
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    void set_values(const std::vector<Int> & ix,
                    const std::vector<Scalar> & y,
                    InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector
    ///
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    template <Int N>
    void set_values(const DenseVector<Int, N> & ix,
                    const DenseVector<Scalar, N> & y,
                    InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector
    ///
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    void set_values(const DynDenseVector<Int> & ix,
                    const DynDenseVector<Scalar> & y,
                    InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector, using a local ordering of the
    /// nodes.
    ///
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    void set_values_local(const std::vector<Int> & ix,
                          const std::vector<Scalar> & y,
                          InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector, using a local ordering of the
    /// nodes.
    ///
    /// @tparam N Number of elements in the vectors
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    template <Int N>
    void set_values_local(const DenseVector<Int, N> & ix,
                          const DenseVector<Scalar, N> & y,
                          InsertMode mode = INSERT_VALUES);

    /// Inserts or adds values into certain locations of a vector, using a local ordering of the
    /// nodes.
    ///
    /// @param ix Indices where to add/insert
    /// @param y Values
    /// @param mode Insertion mode
    void set_values_local(const DynDenseVector<Int> & ix,
                          const DynDenseVector<Scalar> & y,
                          InsertMode mode = INSERT_VALUES);

    /// Sets an option for controlling a vector’s behavior.
    ///
    /// @param op The option
    /// @param flag Turn the option on or off
    void set_option(VecOption op, bool flag);

    Scalar sum() const;

    void zero();

    /// Gets a vector representing part of this vector
    ///
    /// @param is Index set representing portion of X to extract
    Vector get_sub_vector(const IndexSet & is) const;

    /// Restores a subvector extracted by `get_sub_vector`
    ///
    /// @param is Index set representing the subset of X
    /// @param y Subvector being restored
    void restore_sub_vector(const IndexSet & is, Vector & y) const;

    Scalar operator()(Int idx) const;

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator Vec() const;

    static Vector create_seq(MPI_Comm comm, Int n);

    /// Creates a parallel vector.
    ///
    /// @param comm the MPI communicator to use
    /// @param n Local vector length (or PETSC_DECIDE to have calculated if N is given)
    /// @param N global vector length (or PETSC_DETERMINE to have calculated if n is given)
    static Vector create_mpi(MPI_Comm comm, Int n, Int N);

    static void pointwise_min(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_max(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_mult(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_divide(const Vector & w, const Vector & x, const Vector & y);

private:
    Vec vec;
};

template <Int N>
inline void
Vector::set_values(const DenseVector<Int, N> & ix,
                   const DenseVector<Scalar, N> & y,
                   InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValues(this->vec, N, ix.data(), y.data(), mode));
}

template <Int N>
inline void
Vector::set_values_local(const DenseVector<Int, N> & ix,
                         const DenseVector<Scalar, N> & y,
                         InsertMode mode)
{
    CALL_STACK_MSG();
    PETSC_CHECK(VecSetValuesLocal(this->vec, N, ix.data(), y.data(), mode));
}

/// Copy vector `x` into `y`
///
/// @param x Source vector
/// @param y Target vector
void copy(const Vector & x, Vector & y);

/// Compute vector dot product
///
/// @param x First vector
/// @param y Second vector
/// @return Dot product
Scalar dot(const Vector & x, const Vector & y);

/// Computes `y[i] = alpha x[i] + y[i]`
///
/// @param alpha Scalar
/// @param x Vector to scale by `alpha`
void axpy(Vector & y, Scalar alpha, const Vector & x);

/// Computes `y[i] = alpha x[i] + beta y[i]`
///
/// @param alpha Scalar
/// @param beta Scalar
/// @param x Vector scaled by `alpha`
/// @param y Vector accumulated into
void axpby(Vector & y, Scalar alpha, Scalar beta, const Vector & x);

/// Computes `y[i] = x[i] + beta * y[i]`
///
/// @param beta Scalar
/// @param x Unscaled vector
/// @param y Resulting vector
void aypx(Vector & y, Scalar beta, const Vector & x);

/// Computes `z[i] = alpha * x[i] + y[i]
///
/// @param alpha Scalar
/// @param x First vector multiplied by `alpha`
/// @param y Second vector
/// @param w Resulting vector
void waxpy(Vector & w, Scalar alpha, const Vector & x, const Vector & y);

/// Computes `y = y + \sum_i alpha[i] x[i]`
///
/// Length of `alpha` and `x` must be the same
///
/// @param alpha Array of scalars
/// @param x Array of vectors
void maxpy(Vector & y, const std::vector<Scalar> & alpha, const std::vector<Vector> & x);

/// Computes `this = alpha * x + beta * y + gamma * this`
///
/// @param alpha First scalar
/// @param beta Second scalar
/// @param gamma Third scalar
/// @param x First vector
/// @param y Second vector
/// @param z Resulting vector
void
axpbypcz(Vector & z, Scalar alpha, Scalar beta, Scalar gamma, const Vector & x, const Vector & y);

} // namespace godzilla
