// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Error.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "petscsf.h"
#include <vector>

namespace mpi = mpicpp_lite;

namespace godzilla {

template <typename T>
class Array1D;

class StarForest {
public:
    /// Alias for PetscSFNode
    using Node = PetscSFNode;

    class Graph {
    public:
        Graph();
        Graph(Int n_roots, Int n_leaves, const Int * leaves, const Node * remote_leaves);

        Int get_num_roots() const;

        Int get_num_leaves() const;

        const Int * get_leaves() const;

        /// Get leaf at index `idx`
        ///
        /// @param idx Index of the leaf
        /// @return Leaf at index `idx`
        Int get_leaf(Int idx) const;

        const Node * get_remote_leaves() const;

        /// Get remote leaf at index `idx`
        ///
        /// @param idx Index of the remote leaf
        /// @return Remote leaf at index `idx`
        const Node & get_remote_leaf(Int idx) const;

        ///
        Int find_leaf(Int point) const;

        /// Test if graph is empty
        operator bool() const;
        operator bool();

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
    Graph get_graph() const;

    /// Set a parallel star forest
    void set_graph(Int n_roots,
                   Int n_leaves,
                   const std::vector<Int> & ilocal,
                   const std::vector<Node> & iremote) const;

    /// Begin pointwise broadcast with root value being reduced to leaf value, to be concluded with
    /// call to `broadcast_end`
    ///
    /// @tparam T Type of the value to broadcast
    /// @tparam Op Operation to use for reduction
    /// @param root Value to broadcast
    /// @param leaf Value to be reduced with values from each leaf’s respective root
    template <typename T, typename Op>
    void
    broadcast_begin(const T & root, T & leaf, Op) const
    {
        auto dtype = mpi::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFBcastBegin(sf, dtype, &root, &leaf, mpi_op));
    }

    template <typename T, typename Op>
    void
    broadcast_begin(const std::vector<T> & root, std::vector<T> & leaf, Op) const
    {
        auto dtype = mpi::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFBcastBegin(sf, dtype, root.data(), leaf.data(), mpi_op));
    }

    /// End a broadcast and reduce operation started with `broadcast_begin`
    ///
    /// @tparam T Type of the value to broadcast
    /// @tparam Op Operation to use for reduction
    /// @param root Value to broadcast
    /// @param leaf Value to be reduced with values from each leaf’s respective root
    /// @param op Operation to use for reduction
    template <typename T, typename Op>
    void
    broadcast_end(const T & root, T & leaf, Op) const
    {
        auto dtype = mpi::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFBcastEnd(sf, dtype, &root, &leaf, mpi_op));
    }

    template <typename T, typename Op>
    void
    broadcast_end(const std::vector<T> & root, std::vector<T> & leaf, Op) const
    {
        auto dtype = mpi::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFBcastEnd(sf, dtype, root.data(), leaf.data(), mpi_op));
    }

    template <typename T, typename Op>
    void
    reduce_begin(const T * root, T * leaf, Op) const
    {
        CALL_STACK_MSG();
        auto dtype = mpicpp_lite::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFReduceBegin(this->sf, dtype, root, leaf, mpi_op));
    }

    template <typename T, typename Op>
    void
    reduce_begin(const T & root, T & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_begin(&root, &leaf, op);
    }

    template <typename T, typename Op>
    void
    reduce_begin(const std::vector<T> & root, std::vector<T> & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_begin(root.data(), leaf.data(), op);
    }

    template <typename T, typename Op>
    void
    reduce_begin(const Array1D<T> & root, Array1D<T> & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_begin(root.get_data(), leaf.get_data(), op);
    }

    template <typename T, typename Op>
    void
    reduce_end(const T * root, T * leaf, Op) const
    {
        CALL_STACK_MSG();
        auto dtype = mpicpp_lite::mpi_datatype<T>();
        auto mpi_op = mpi::op::provider<T, Op, mpi::op::Operation<Op, T>::is_native::value>::op();
        PETSC_CHECK(PetscSFReduceEnd(this->sf, dtype, root, leaf, mpi_op));
    }

    template <typename T, typename Op>
    void
    reduce_end(const T & root, T & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_end(&root, &leaf, op);
    }

    template <typename T, typename Op>
    void
    reduce_end(const std::vector<T> & root, std::vector<T> & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_end(root.data(), leaf.data(), op);
    }

    template <typename T, typename Op>
    void
    reduce_end(const Array1D<T> & root, Array1D<T> & leaf, Op op) const
    {
        CALL_STACK_MSG();
        reduce_end(root.get_data(), leaf.get_data(), op);
    }

    /// View a star forrest
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    /// typecast operator so we can use our class directly with PETSc API
    operator PetscSF() const;

private:
    PetscSF sf;
};

} // namespace godzilla
