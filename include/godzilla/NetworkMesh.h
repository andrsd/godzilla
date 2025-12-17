// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Range.h"
#include "godzilla/Mesh.h"
#include "godzilla/Matrix.h"

namespace godzilla {

class MeshNetwork : public godzilla::Mesh {
public:
    MeshNetwork(const mpi::Communicator & comm);
    explicit MeshNetwork(DM dm);

    Int register_component(String name, std::size_t size);

    void add_component(Int p, Int key, void * comp, Int n_vars);

    void * component(Int p);

    /// Gets the number of subnetworks
    ///
    /// @return local and global number of subnetworks
    std::tuple<Int, Int> num_sub_networks() const;

    /// Sets the number of subnetworks
    ///
    /// @param n local number of subnetworks
    /// @param N global number of subnetworks
    void set_num_sub_networks(Int n, Int N);

    /// Add a subnetwork
    ///
    /// @param name Name of the subnetwork
    /// @param edge_list list of edges for this subnetwork, this is a one dimensional array with
    ///                  pairs of entries being the two vertices (in global numbering of the
    ///                  vertices) of each edge, `e0v0 e0v1 e1v0 e1v1 ...`
    Int add_sub_network(String name, std::vector<Int> & edge_list);

    ///
    void create();

    /// Get the number of components at a vertex/edge
    int num_components(Int p) const;

    /// Get the local and global number of vertices for the entire network.
    std::tuple<Int, Int> num_vertices() const;

    /// Get the bounds [start, end) for the local vertices
    Range vertex_range() const;

    /// Return the supporting edges for this vertex point
    std::vector<Int> supporting_edges(Int vertex);

    /// Returns `true` if the vertex is a ghost vertex
    bool is_ghost_vertex(Int p) const;

    /// Get the local and global number of edges for the entire network.
    std::tuple<Int, Int> num_edges() const;

    /// Get the bounds [start, end) for the local edges
    Range edge_range() const;

    /// Return the connected vertices for this edge point
    std::array<Int, 2> connected_vertices(Int edge);

    /// Sets up the bare layout (graph) for the network
    void layout_set_up();

    /// Distributes the network and moves associated component data
    void distribute(Int overlap = 0);

    void finalize_components();

    /// Get the offset for accessing the variables associated with the given edge from the local
    /// subvector
    Int edge_offset(Int p) const;

    /// Get the global numbering for the edge on the network
    ///
    /// @param p Edge point
    /// @return the global numbering for the edge
    Int global_edge_index(Int p) const;

    /// Get the global offset for accessing the variables associated with a component for the given
    /// vertex/edge from the global vector
    ///
    /// @param p The edge or vertex point
    /// @param comp_num component number; use ALL_COMPONENTS if no specific component is requested
    /// @return The global offset
    Int global_vec_offset(Int p, Int comp_num) const;

    /// Get the global numbering for the vertex on the network
    ///
    /// @param p Vertex point
    /// @return The global numbering for the vertex
    Int global_vertex_index(Int p) const;

    /// Get the offset for accessing the variables associated with a component at the given
    /// vertex/edge from the local vector
    ///
    /// @param p The edge or vertex point
    /// @param comp_num Component number; use ALL_COMPONENTS if no specific component is requested
    /// @return The local offset
    Int local_vec_offset(Int p, Int comp_num) const;

    /// Get the offset for accessing the variables associated with the given vertex from the local
    /// subvector
    ///
    /// @param p The vertex point
    /// @return The offset
    Int vertex_offset(Int p) const;

    /// Sets user-provided Jacobian matrices for this edge to the network
    ///
    /// @param p The edge point
    /// @param J Array of Jacobian submatrices for this edge point:
    ///          - J[0] this edge
    ///          - J[1] and J[2]: connected vertices
    void edge_set_matrix(Int p, const std::array<Matrix, 3> & J);

    /// Sets user-provided Jacobian matrix for this vertex to the network
    ///
    /// @param p The vertex point
    /// @param J array of Jacobian (size = 2*(num of supporting edges) + 1) submatrices for this
    ///          vertex point:
    ///          - J[0]: this vertex
    ///          - J[1+2i]: i-th supporting edge
    ///          - J[1+2i+1]: i-th connected vertex
    void vertex_set_matrix(Int p, const std::vector<Matrix> & J);

    void set_user_jacobian(bool eflg, bool vflg);

private:
    DM netw_;
};

} // namespace godzilla
