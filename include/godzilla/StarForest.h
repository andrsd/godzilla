// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "petscsf.h"
#include <vector>

namespace godzilla {

class StarForest {
public:
    /// Alias for PetscSFNode
    using Node = PetscSFNode;

    class Graph {
    public:
        Graph();
        Graph(Int n_roots, Int n_leaves, const Int * leaves, const Node * remote_leaves);

        [[nodiscard]] Int get_num_roots() const;

        [[nodiscard]] Int get_num_leaves() const;

        [[nodiscard]] const Int * get_leaves() const;

        /// Get leaf at index `idx`
        ///
        /// @param idx Index of the leaf
        /// @return Leaf at index `idx`
        [[nodiscard]] Int get_leaf(Int idx) const;

        [[nodiscard]] const Node * get_remote_leaves() const;

        /// Get remote leaf at index `idx`
        ///
        /// @param idx Index of the remote leaf
        /// @return Remote leaf at index `idx`
        [[nodiscard]] const Node & get_remote_leaf(Int idx) const;

        ///
        [[nodiscard]] Int find_leaf(Int point) const;

        /// Test if graph is empty
        operator bool() const;

    private:
        Int n_roots, n_leaves;
        const Int * leaves;
        const Node * remote_leaves;
    };

    StarForest();
    StarForest(PetscSF sf);

    /// Create a star forest communication context
    void create(MPI_Comm comm);

    /// Destroy the star forest
    void destroy();

    /// Reset a star forest so that different sizes or neighbors can be used
    void reset();

    /// Set up communication structures for the star forrest, after this is done it may be used to
    /// perform communication
    void set_up();

    /// Get the graph specifying a parallel star forest
    [[nodiscard]] Graph get_graph() const;

    /// Set a parallel star forest
    void set_graph(Int n_roots,
                   Int n_leaves,
                   const std::vector<Int> & ilocal,
                   const std::vector<Node> & iremote) const;

    /// View a star forrest
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    /// typecast operator so we can use our class directly with PETSc API
    operator PetscSF() const;

private:
    PetscSF sf;
};

} // namespace godzilla
