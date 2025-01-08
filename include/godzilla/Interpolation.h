// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Error.h"
#include "godzilla/Types.h"
#include "godzilla/Vector.h"
#include "petscdm.h"

namespace godzilla {

class Interpolation {
public:
    /// Construct an interpolation object
    Interpolation();

    ~Interpolation();

    /// Creates an interpolation object
    void create(MPI_Comm comm);

    /// Destroys the interpolation object
    void destroy();

    /// Add points at which we will interpolate the fields
    ///
    /// @param n The number of points
    /// @param points The coordinates of the points, an array of size `n * dim`
    void add_points(Int n, const Real points[]);

    void add_points(const std::vector<Real> & points);

    /// Gets a Vec with the coordinates of each interpolation point
    ///
    /// @return The coordinates of the interpolation points
    Vector get_coordinates() const;

    /// Gets the spatial dimension for the interpolation context
    ///
    /// @return The spatial dimension
    Int get_dim() const;

    /// Gets the number of fields interpolated at a point
    ///
    /// @return The number of fields
    Int get_dof() const;

    /// Gets a `Vector` which can hold all the interpolated field values
    ///
    /// @return A vector capable of holding the interpolated field values
    ///
    /// @note This vector should be returned using `RestoreVector()
    Vector get_vector();

    /// Restores the vector returned by `get_vector()`
    ///
    /// @param v The vector to restore
    void restore_vector(Vector & v);

    /// Sets the spatial dimension for the interpolation context
    ///
    /// @param dim The spatial dimension
    void set_dim(Int dim);

    /// Sets the number of fields interpolated at a point for the interpolation context
    ///
    /// @param dof The number of fields
    void set_dof(Int dof);

    /// Compute spatial indices for point location during interpolation
    ///
    /// @param dm The DM object
    /// @param redundant_points If `true`, all processes are passing in the same array of points.
    ///        Otherwise, points need to be communicated among processes.
    /// @param ignore_outside_domain If `true`, ignore points outside the domain, otherwise return
    ///        an error
    void set_up(DM dm, bool redundant_points, bool ignore_outside_domain);

    /// Using the input from `dm` and `x`, calculate interpolated field values at the interpolation
    /// points.
    ///
    /// @param dm The DM object
    /// @param x The local vector containing the field to be interpolated
    /// @param values The vector containing the interpolated values, obtained with `get_vector()`
    void evaluate(DM dm, const Vector & x, Vector & values);

private:
    DMInterpolationInfo info;
};

} // namespace godzilla
