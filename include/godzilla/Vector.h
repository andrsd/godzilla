// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
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
    [[nodiscard]] std::string get_type() const;

    Scalar * get_array();
    [[nodiscard]] const Scalar * get_array_read() const;
    void restore_array(Scalar * arr);
    void restore_array_read(const Scalar * arr) const;

    /// Returns the global number of elements of the vector
    [[nodiscard]] Int get_size() const;
    /// Returns the local number of elements of the vector
    [[nodiscard]] Int get_local_size() const;
    void get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const;

    void abs();
    [[nodiscard]] Scalar dot(const Vector & y) const;
    void scale(Scalar alpha);
    void duplicate(Vector & b) const;
    [[nodiscard]] Vector duplicate() const;

    /// Copy this vector into `y`
    ///
    /// @param y Vector to copy our values into
    void copy(Vector & y) const;

    void normalize();
    void shift(Scalar shift);
    [[nodiscard]] Scalar min() const;
    [[nodiscard]] Scalar max() const;
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
    /// @param x Vetor to scale by `alpha`
    void axpy(Scalar alpha, const Vector & x);

    /// Computes `this[i] = alpha x[i] + beta y[i]`
    ///
    /// @param alpha Scalar
    /// @param beta Scalar
    /// @param x Vector sclaed by `alpha`
    void axpby(Scalar alpha, Scalar beta, const Vector & x);

    /// Computes `this[i] = x[i] + beta * this[i]`
    ///
    /// @param beta Scalar
    /// @param x Unscaled vector
    void aypx(Scalar beta, const Vector & x);

    /// Computes `this[i] = alpha * x[i] + y[i]
    ///
    /// @param alpha Scalar
    /// @param x First vector multiplied by `alpha`
    /// @param y Second vector
    void waxpy(Scalar alpha, const Vector & x, const Vector & y);

    /// Computes `this = this + \sum_i alpha[i] x[i]`
    ///
    /// Length of `alpha` and `x` must be the same
    ///
    /// @param alpha Array of scalars
    /// @param x Array of vectors
    void maxpy(const std::vector<Scalar> & alpha, const std::vector<Vector> & x);

    /// Computes `this = alpha * x + beta * y + gamma * this`
    ///
    /// @param alpha First scalar
    /// @param beta Second scalar
    /// @param gamma Third scalar
    /// @param x First vector
    /// @param y Second vector
    void axpbypcz(Scalar alpha, Scalar beta, Scalar gamma, const Vector & x, const Vec & y);

    void reciprocal();

    void set(Scalar alpha);
    void set_sizes(Int n, Int N = PETSC_DECIDE);
    void set_block_size(Int bs);
    void set_type(VecType method);

    void set_value(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_value_local(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_values(const std::vector<Int> & ix,
                    const std::vector<Scalar> & y,
                    InsertMode mode = INSERT_VALUES);
    void set_values_local(const std::vector<Int> & ix,
                          const std::vector<Scalar> & y,
                          InsertMode mode = INSERT_VALUES);

    /// Sets an option for controlling a vector’s behavior.
    ///
    /// @param op The option
    /// @param flag Turn the option on or off
    void set_option(VecOption op, bool flag);

    [[nodiscard]] Scalar sum() const;

    void zero();

    Scalar operator()(Int idx) const;

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator Vec() const;

    static Vector create_seq(MPI_Comm comm, Int n);

    static void pointwise_min(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_max(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_mult(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_divide(const Vector & w, const Vector & x, const Vector & y);

private:
    Vec vec;
};

} // namespace godzilla
