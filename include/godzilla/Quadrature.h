// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Types.h"
#include "petscdt.h"

namespace godzilla {

enum class SimplexQuadratureType {
    DEFAULT = PETSCDTSIMPLEXQUAD_DEFAULT,
    CONIC = PETSCDTSIMPLEXQUAD_CONIC,
    MINSYM = PETSCDTSIMPLEXQUAD_MINSYM
};

class Quadrature : public PetscObjectWrapper<PetscQuadrature> {
public:
    Quadrature();
    Quadrature(PetscQuadrature quad);

    /// Create quadrature object
    void create(MPI_Comm comm);

    /// Destroy quadrature object
    void destroy();

    /// Duplicate the object into `r`
    ///
    /// @param r New quadrature object with the duplicate
    void duplicate(Quadrature & r);

    /// Duplicate the object
    Quadrature duplicate() const;

    /// Get the spatial dimension
    ///
    /// @return The spatial dimension
    Dimension get_dim() const;

    /// Get number of components
    ///
    /// @return the number of components
    Int get_num_components() const;

    /// The number of quadrature points
    ///
    /// @return The number of quadrature points
    Int get_num_points() const;

    /// Get quadrature point weights
    ///
    /// @return Array of quadrature point weights
    const Real * get_weights() const;

    /// Get quadrature points
    ///
    /// @return Array of quadrature points
    const Real * get_points() const;

    /// Get order
    ///
    /// @return The order of the quadrature, i.e. the highest degree polynomial that is exactly
    /// integrated
    Int get_order() const;

    /// Determine whether two quadratures are equivalent
    ///
    /// @param q Quadrature to compare to
    /// @return `true` if quadratures are the same, `false` otherwise
    bool equal(const Quadrature & q) const;

    /// Create a tensor-product Gauss quadrature
    ///
    /// @param dim Spatial dimension
    /// @param n_comp Number of components
    /// @param n_points Number of points in one dimension
    /// @param a Left end of interval (often `-1`)
    /// @param b Right end of interval (often `1`)
    /// @return Quadrature object
    static Quadrature create_gauss_tensor(Dimension dim, Int n_comp, Int n_points, Real a, Real b);

    /// Create a quadrature rule for a simplex that exactly integrates polynomials up to a given
    /// degree.
    ///
    /// @param dim Spatial dimension of the simplex (1 = segment, 2 = triangle, 3 = tetrahedron)
    /// @param degree The largest polynomial degree that is required to be integrated exactly
    /// @param type Indicating the type of quadrature rule
    static Quadrature create_simplex(Dimension dim,
                                     Int degree,
                                     SimplexQuadratureType type = SimplexQuadratureType::DEFAULT);

    /// Create Stroud conical quadrature for a simplex
    ///
    /// @param dim Spatial dimension
    /// @param n_comp Number of components
    /// @param n_points Number of points in one dimension
    /// @param a Left end of interval (often `-1`)
    /// @param b Right end of interval (often `1`)
    /// @return Quadrature object
    static Quadrature
    create_stroud_conical(Dimension dim, Int n_comp, Int n_points, Real a = -1, Real b = 1);
};

} // namespace godzilla
