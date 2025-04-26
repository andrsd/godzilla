// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Matrix.h"
#include "godzilla/IndexSet.h"
#include "petscmat.h"

namespace godzilla {

class Partitioning {
public:
    Partitioning();
    Partitioning(MatPartitioning mp);

    void create(MPI_Comm comm);
    void destroy();

    /// Gets the partitioning method name
    ///
    /// @return Partitining type
    std::string get_type() const;

    /// Sets the type of partitioner to use
    ///
    /// @param type Partitioner name
    void set_type(const std::string & type) const;

    /// Set how many partitions need to be created; by default this is one per processor. Certain
    /// partitioning schemes may in fact only support that option.
    ///
    /// @param n The number of partitions
    void set_n_parts(Int n);

    /// Query whether or not the edge weights are used
    ///
    /// @return Whether or not the edge weights are used
    bool get_use_edge_weights() const;

    /// Set a flag to indicate whether or not to use edge weights
    ///
    /// @param flag The flag indicating whether or not to use edge weights. By default, no edge
    ///             weights will be used, that is, `use_edge_weights` is set to `false`. If
    ///             `use_edge_weights` is set to `true`, users need to make sure legal edge weights
    ///             are stored in an ADJ matrix.
    void set_use_edge_weights(bool flag);

    /// Gets a partitioning for the graph represented by a sparse matrix
    ///
    /// @return The partitioning. For each local node this tells the MPI rank that the node is
    /// assigned to.
    IndexSet apply();

    /// Improves the quality of the partitioning
    ///
    /// @param partitioning The original partitioning. For each local node this tells the processor
    /// number that the node is assigned to.
    void improve(IndexSet & partitioning);

    /// Sets the adjacency graph (matrix) of the thing to be partitioned.
    ///
    /// @param adj The adjacency matrix, this can be any `MatType` but the natural representation is
    /// `MATMPIADJ`
    void set_adjacency(const Matrix & adj);

    /// Sets the number of weights per vertex
    ///
    /// @param n The number of weights
    void set_number_vertex_weights(Int n);

    /// Sets the weights for vertices for a partitioning
    ///
    /// @param weights The weights, on each process this array must have the same size as the number
    ///                of local rows times the value passed with `set_number_vertex_weights` or 1 if
    ///                that is not provided
    void set_vertex_weights(const std::vector<Int> & weights);
    void set_vertex_weights(const Int weights[]);

    /// Sets the weights for each partition
    ///
    /// @param weights An array of size `nparts` that is used to specify the fraction of vertex
    ///        weight that should be distributed to each sub-domain for the balance constraint. If
    ///        all of the sub-domains are to be of the same size, then each of the nparts elements
    ///        should be set to a value of `1/nparts`. Note that the sum of all of the weights
    ///        should be one.
    void set_partition_weights(const std::vector<Real> & weights);
    void set_partition_weights(const Real weights[]);

    /// Prints the partitioning data structure.
    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator MatPartitioning() const;
    operator MatPartitioning();

private:
    MatPartitioning matp;

public:
    static const char * AVERAGE;
    static const char * SQUARE;
    static const char * PARMETIS;
    static const char * PARTY;
    static const char * CHACO;
    static const char * PTSCOTCH;
};

} // namespace godzilla
