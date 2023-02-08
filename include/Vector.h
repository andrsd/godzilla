#pragma once

#include <vector>
#include "mpi.h"
#include "petscvec.h"
#include "Types.h"

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
    void restore_array(Scalar * arr);

    /// Returns the global number of elements of the vector
    Int get_size() const;
    /// Returns the local number of elements of the vector
    Int get_local_size() const;
    void get_values(const std::vector<Int> & idx, std::vector<Scalar> & y) const;

    void abs();
    Scalar dot(const Vector & y) const;
    void scale(Scalar alpha);
    void duplicate(Vector & b) const;
    void copy(Vector & y);
    void normalize();
    void shift(Scalar shift);
    Scalar min() const;
    Scalar max() const;
    void chop(Real tol);
    void axpy(Scalar alpha, const Vector & x);
    void aypx(Scalar beta, const Vector & x);

    void set(Scalar alpha);
    void set_sizes(Int n, Int N = PETSC_DECIDE);
    void set_type(VecType method);

    void set_value(Int row, Scalar value, InsertMode mode = INSERT_VALUES);
    void set_values(const std::vector<Int> & ix,
                    const std::vector<Scalar> & y,
                    InsertMode mode = INSERT_VALUES);
    void set_values_local(const std::vector<Int> & ix,
                          const std::vector<Scalar> & y,
                          InsertMode mode = INSERT_VALUES);

    Scalar sum() const;

    void zero();

    Scalar operator[](Int idx) const;

    explicit operator Vec() const;

    static Vector create_seq(MPI_Comm comm, Int n);

    static void pointwise_min(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_max(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_mult(const Vector & w, const Vector & x, const Vector & y);
    static void pointwise_divide(const Vector & w, const Vector & x, const Vector & y);

private:
    Vec vec;
};

} // namespace godzilla
