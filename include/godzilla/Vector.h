#pragma once

#include <vector>
#include "mpi.h"
#include "petscvec.h"
#include "godzilla/Types.h"

namespace godzilla {

class Vector {
public:
    Vector();
    Vector(MPI_Comm comm);
    Vector(Vec vec);
    virtual ~Vector();

    void create(MPI_Comm comm);
    void destroy();
    void set_name(const std::string & name);

    void assembly_begin();
    void assembly_end();

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
    Scalar dot(const Vector & y) const;
    void scale(Scalar alpha);
    void duplicate(Vector & b) const;

    /// Copy this vector into `y`
    ///
    /// @param y Vector to copy our values into
    void copy(Vector & y) const;

    void normalize();
    void shift(Scalar shift);
    Scalar min() const;
    Scalar max() const;
    void chop(Real tol);

    /// Computes `this[i] = alpha x[i] + this[i]`
    ///
    /// @param alpha Scalar
    /// @param x Vetor to scale by `alpha`
    void axpy(Scalar alpha, const Vector & x);

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

    void reciprocal();

    void set(Scalar alpha);
    void set_sizes(Int n, Int N = PETSC_DECIDE);
    void set_type(VecType method);

    void set_value(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_value_local(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_values(const std::vector<Int> & ix,
                    const std::vector<Scalar> & y,
                    InsertMode mode = INSERT_VALUES);
    void set_values_local(const std::vector<Int> & ix,
                          const std::vector<Scalar> & y,
                          InsertMode mode = INSERT_VALUES);

    Scalar sum() const;

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
