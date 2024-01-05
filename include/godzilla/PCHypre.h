// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Preconditioner.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"

namespace godzilla {

class PCHypre : public Preconditioner {
public:
    enum Type { EUCLID, PILUT, PARASAILS, BOOMERAMG, AMS, ADS };

    PCHypre();
    PCHypre(PC pc);

    /// Creates a preconditioner
    ///
    /// @param comm MPI communicator
    void create(MPI_Comm comm);

    /// Sets which HYPRE preconditioner to use
    ///
    /// @param type HYPRE preconditioner type
    void set_type(Type type);

    /// Gets which HYPRE preconditioner is used
    ///
    /// @return HYPRE preconditioner type
    Type get_type() const;

    /// Set the list of interior nodes to a zero-conductivity region for PCHYPRE of type `AMS`
    ///
    /// @param interior Vector. Node is interior if its entry in the array is 1.0.
    void ams_set_interior_nodes(const Vector & interior);

    /// Set vector Poisson matrix for PCHYPRE of type `AMS`
    ///
    /// @param A The matrix
    void set_alpha_poisson_matrix(const Matrix & A);

    /// Set Poisson matrix for PCHYPRE of type `AMS`
    ///
    /// @param A The matrix
    void set_beta_poisson_matrix(const Matrix & A);

    /// Set discrete curl matrix for PCHYPRE type of `ADS`
    ///
    /// @param C The discrete curl
    void set_discrete_curl(const Matrix & C);

    /// Set discrete gradient matrix for PCHYPRE type of `AMS` or `ADS`
    ///
    /// @param G The discrete gradient
    void set_discrete_gradient(const Matrix & G);

    /// Set the representation of the constant vector fields in the edge element basis for PCHYPRE
    /// of type `AMS` (for 2D)
    ///
    /// @param oz Vector representing (1,0)
    /// @param zo Vector representing (0,1)
    void set_edge_constant_vectors(const Vector & oz, const Vector & zo);

    /// Set the representation of the constant vector fields in the edge element basis for PCHYPRE
    /// of type `AMS` (for 3D)
    ///
    /// @param ozz Vector representing (1,0,0)
    /// @param zoz Vector representing (0,1,0)
    /// @param zzo Vector representing (0,0,1)
    void set_edge_constant_vectors(const Vector & ozz, const Vector & zoz, const Vector & zzo);
};

} // namespace godzilla
