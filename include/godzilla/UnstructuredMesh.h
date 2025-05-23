// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Mesh.h"
#include "godzilla/Range.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Vector.h"
#include "godzilla/Section.h"
#include "godzilla/DenseVector.h"
#include "godzilla/StarForest.h"
#include <map>

namespace godzilla {

class Partitioner;

/// Unstructured mesh (wrapper around DMPLEX + extra stuff)
///
class UnstructuredMesh : public Mesh {
public:
    explicit UnstructuredMesh(const mpi::Communicator & comm);
    explicit UnstructuredMesh(DM dm);
    ~UnstructuredMesh() override;

    UnstructuredMesh clone() const;

    /// Get the `Label` recording the depth of each point
    ///
    /// @return The `Label` recording point depth
    Label get_depth_label() const;

    /// Return the number of mesh vertices
    Int get_num_vertices() const;

    /// Get range of vertex indices
    ///
    /// @return Range of vertex indices
    Range get_vertex_range() const;

    /// Return the number of mesh faces
    Int get_num_faces() const;

    /// Get range of face indices
    ///
    /// @return Range of face indices
    Range get_face_range() const;

    /// Return the number of mesh cells (interior)
    ///
    /// @return Number of mesh cells (interior)
    Int get_num_cells() const;

    /// Return the number of all mesh cells (interior + ghosted)
    ///
    /// @return Number of all mesh cells (interior + ghosted)
    Int get_num_all_cells() const;

    /// Get range of cell indices (interior only)
    ///
    /// @return Range of cell indices
    Range get_cell_range() const;

    /// Get range of all cell indices (interior + ghosted)
    ///
    /// @param Range of all cell indices (interior + ghosted)
    Range get_all_cell_range() const;

    /// Get range of cell indices owned by this processor (must call `create_my_cells_label` first)
    ///
    /// @return Range of cell indices
    Range get_my_cell_range() const;

    /// Get range of ghost cell indices
    ///
    /// @param Range of ghost cell indices
    Range get_ghost_cell_range() const;

    /// Get index set with all cells
    ///
    /// @return Index set with all cells
    IndexSet get_all_cells() const;

    /// Get index set with facets
    ///
    /// @return Index set with facets
    IndexSet get_facets() const;

    /// Return the interval for all mesh points [start, end)
    ///
    /// @return Range of mesh points
    Range get_chart() const;

    /// Set the interval for all mesh points `[start, end)`
    ///
    /// @param start The first mesh point
    /// @param end The upper bound for mesh points
    void set_chart(Int start, Int end);

    /// Get cell type
    ///
    /// @param cell Cell index
    /// @return Cell type
    PolytopeType get_cell_type(Int cell) const;

    /// Calculate an index for the default section
    void create_closure_index();

    /// Calculate an index for the given Section for the closure operation
    ///
    /// @param section The section describing the layout in the local vector
    void create_closure_index(const Section & section);

    /// Get connectivity
    ///
    /// @param point Point with must lie in the chart
    /// @return Point connectivity
    std::vector<Int> get_connectivity(Int point) const;

    /// Return the points on the out-edges for this point
    ///
    /// @param point Point with must lie in the chart
    /// @return Points which are on the out-edges for point `p`
    std::vector<Int> get_support(Int point) const;

    /// Return the number of out-edges for this point in the DAG
    ///
    /// @param point Point with must lie in the chart
    /// @return The support size for `point`
    Int get_support_size(Int point) const;

    /// Return the points on the in-edges for this point
    ///
    /// @param point Point with must lie in the chart
    /// @return Points which are on the out-edges for point `p`
    std::vector<Int> get_cone(Int point) const;

    /// Return the number of in-edges for this point in the DAG
    ///
    /// @param point The point, which must lie in the chart set with `set_chart`
    /// @return The cone size for `point`
    Int get_cone_size(Int point) const;

    /// Expand each given point into its cone points and do that recursively until we end up just
    /// with vertices.
    ///
    /// @param points IndexSet of points with must lie in the chart
    /// @return Vertices recursively expanded from input points
    IndexSet get_cone_recursive_vertices(IndexSet points) const;

    /// Set the number of in-edges for this point in the DAG
    ///
    /// @param point The point, which must lie in the chart set with `set_chart`
    /// @param size The cone size for point `p`
    void set_cone_size(Int point, Int size);

    /// Set the points on the in-edges for this point in the DAG; that is these are the points that
    /// cover the specific point
    ///
    /// @param point The point, which must lie in the chart set with `set_chart`
    /// @param cone An array of points which are on the in-edges for point `p`
    void set_cone(Int point, const std::vector<Int> & cone);

    /// Get the depth of a given point
    ///
    /// @param point The point
    /// @retrun The depth of the point
    Int get_point_depth(Int point) const;

    /// Is the first cell in the mesh a simplex?
    ///
    /// @return true if cell is a simplex, otherwise false
    bool is_simplex() const;

    /// Get cell set name
    ///
    /// @param id The ID of the cell set
    /// @return Cell set name
    const std::string & get_cell_set_name(Int id) const;

    /// Get cell set ID
    ///
    /// @param name The name of a cell sel
    /// @return Cell set ID
    Int get_cell_set_id(const std::string & name) const;

    /// Get number of cell sets
    ///
    /// @return Number of cell sets
    Int get_num_cell_sets() const;

    /// Get cell sets
    ///
    /// @return Cell sets
    const std::map<Int, std::string> & get_cell_sets() const;

    /// Create cell set. Takes the ID and creates a label with `name` corresponding to the ID.
    ///
    /// @param id Cell set ID
    /// @param name Name of the cell set to create
    void create_cell_set(Int id, const std::string & name);

    /// Create an entry in cell set name map
    ///
    /// @param id Cell set ID
    /// @param name Cell set name
    void set_cell_set_name(Int id, const std::string & name);

    /// Set the polytope type of a given cell
    ///
    /// @param cell The cell
    /// @param cell_type The polytope type of the cell
    void set_cell_type(Int cell, PolytopeType cell_type);

    /// Get face set name
    ///
    /// @param id The ID of the face set
    /// @return Facet name
    const std::string & get_face_set_name(Int id) const;

    /// Get number of face sets
    ///
    /// @return Number of face sets
    Int get_num_face_sets() const;

    /// Get face sets
    ///
    /// @return Face sets
    const std::map<Int, std::string> & get_face_sets() const;

    /// Check if mesh has a label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return true if label exists, false otherwise
    bool has_face_set(const std::string & name) const;

    /// Get label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return Label associated with face set name
    Label get_face_set_label(const std::string & name) const;

    void create_face_set_labels(const std::map<Int, std::string> & names);

    /// Create face set. Takes the ID and creates a label with `name` corresponding to the ID.
    ///
    /// @param id Face set ID
    /// @param name Name of the face set to create
    void create_face_set(Int id, const std::string & name);

    /// Create an entry in face set name map
    ///
    /// @param id Face set ID
    /// @param name Face set name
    void set_face_set_name(Int id, const std::string & name);

    /// Get vertex set name
    ///
    /// @param id The ID of the vertex set
    /// @return Vertex set name
    const std::string & get_vertex_set_name(Int id) const;

    /// Get number of vertex sets
    ///
    /// @return Number of vertex sets
    Int get_num_vertex_sets() const;

    /// Get vertex sets
    ///
    /// @return Vertex sets
    const std::map<Int, std::string> & get_vertex_sets() const;

    /// Check if mesh has a label corresponding to a vertex set name
    ///
    /// @param name The name of the vertex set
    /// @return true if label exists, false otherwise
    bool has_vertex_set(const std::string & name) const;

    /// Get label corresponding to a vertex set name
    ///
    /// @param name The name of the face set
    /// @return Label associated with face set name
    Label get_vertex_set_label(const std::string & name) const;

    void create_vertex_set_labels(const std::map<Int, std::string> & names);

    /// Create vertex set. Takes the ID and creates a label with `name` corresponding to the ID.
    ///
    /// @param id Vertex set ID
    /// @param name Name of the vertex set to create
    void create_vertex_set(Int id, const std::string & name);

    /// Create an entry in vertex set name map
    ///
    /// @param id Vertex set ID
    /// @param name Vertex set name
    void set_vertex_set_name(Int id, const std::string & name);

    /// Set the mesh partitioner
    ///
    /// @param part The partitioner
    void set_partitioner(const Partitioner & part);

    /// Distributes the mesh and any associated sections
    ///
    /// @param overlap The overlap of partitions
    void distribute(Int overlap);

    /// Find out whether this mesh is distributed
    ///
    /// @return `true` if the mesh is distributed, `false` otherwise
    bool is_distributed() const;

    /// Construct ghost cells which connect to every boundary face
    ///
    void construct_ghost_cells();

    /// Compute the volume for a given cell
    ///
    /// @param cell The cell
    /// @param vol The cell volume
    /// @param centroid The cell centroid
    /// @param normal The cell normal, if appropriate
    void compute_cell_geometry(Int cell, Real * vol, Real centroid[], Real normal[]) const;

    /// Compute the volume for a given cell
    ///
    /// @param cell The cell
    /// @return The cell volume
    Real compute_cell_volume(Int cell) const;

    /// Compute a map of cells common to a vertex
    ///
    /// @return A mapping [vertex index -> list of cell indices]
    const std::map<Int, std::vector<Int>> & common_cells_by_vertex();

    /// Mark all faces on the boundary
    ///
    /// @param val The marker value
    /// @param label The `Label` marking boundary faces with the given value
    void mark_boundary_faces(Int val, Label & label);

    /// Get the encoding of the parallel section point overlap
    StarForest get_point_star_forest() const;

    void set_point_star_forest(const StarForest & sf);

    /// Take in a cell-vertex mesh and return one with all intermediate faces, edges, etc.
    void interpolate();

    /// Create support (out-edge) information from cone (in-edge) information
    void symmetrize();

    /// Computes the strata for all points in the mesh
    void stratify();

    /// Get an array for the join of the set of points
    ///
    /// @param points The input points
    /// @return The points in the join
    std::vector<Int> get_full_join(const std::vector<Int> & points);

    /// Get coordinates of a mesh vertex
    ///
    /// @return Vector with dimension components containing vertex coordinates
    std::vector<Real> get_vertex_coordinates(Int pt) const;

    /// Get a global cell numbering for all cells on this process
    ///
    /// @return Global cell numbers on this process
    IndexSet get_cell_numbering() const;

    /// Create a label named "my_cells" that marks all cells owned by this processor with 1 and
    /// cells owned by other processors with 0.
    void create_my_cells_label();

    /// Create a label named "my_facets" that marks all facets owned by this processor with 1 and
    /// facets owned by other processors with 0.
    void create_my_facets_label();

    /// Check if cell is owned by this process
    ///
    /// @param cell The cell to check
    /// @return `true` if the cell is owned by this process, `false` otherwise
    bool is_my_cell(Int cell) const;

private:
    /// Cell set names
    std::map<Int, std::string> cell_set_names;
    /// Cell set IDs
    std::map<std::string, Int> cell_set_ids;

    /// Face set names
    std::map<Int, std::string> face_set_names;
    /// Face set IDs
    std::map<std::string, Int> face_set_ids;

    /// Vertex set names
    std::map<Int, std::string> vertex_set_names;
    /// Vertex set IDs
    std::map<std::string, Int> vertex_set_ids;

    /// Cells common to a vertex
    std::map<Int, std::vector<Int>> common_cells_by_vtx;
    /// Flag indicating if `common_cells_by_vtx` was computed
    bool common_cells_by_vtx_computed;

public:
    static int get_num_cell_nodes(PolytopeType cell_type);

    /// Build from a list of vertices for each cell (common mesh generator output)
    ///
    /// @param comm MPI communicator
    /// @param dim The topological dimension of the mesh
    /// @param n_corners The number of vertices for each cell
    /// @param cells An array of the vertices for each cell - must be a multiple of `n_corners`
    /// @param space_dim The spatial dimension used for coordinates
    /// @param vertices An array of the coordinates of each vertex - must be a multiple of
    ///                 `space_dim`
    /// @param interpolate Flag indicating that intermediate mesh entities (faces, edges) should be
    ///                    created automatically
    ///
    /// NOTES:
    /// - only process 0 takes in the input
    static UnstructuredMesh * build_from_cell_list(const mpi::Communicator & comm,
                                                   Int dim,
                                                   Int n_corners,
                                                   const std::vector<Int> & cells,
                                                   Int space_dim,
                                                   const std::vector<Real> & vertices,
                                                   bool interpolate);

    /// Get polytope dimension
    ///
    /// @param type Cell type
    static Int get_polytope_dim(PolytopeType type);

    /// Flips cell orientations since DMPLEX stores some of them internally with outward normals.
    ///
    /// @param type Cell type
    /// @param cone Cone to invert
    static void invert_cell(PolytopeType type, std::vector<Int> & cone);
};

/// Get index set with points for a given label
///
/// @param label Label to extract points from
/// @return IndexSet containing the points
inline IndexSet
points_from_label(const Label & label)
{
    [[maybe_unused]] auto is = label.get_value_index_set();
    auto ids = label.get_values();
    if (ids.size() > 0)
        return label.get_stratum(ids[0]);
    else
        return {};
}

/// Compute boundary vertices from boundary facets
///
/// @param mesh Unstructured mesh
/// @param facets Boundary facets
/// @return Boundary vertices
inline IndexSet
boundary_vertices(const UnstructuredMesh * mesh, const IndexSet & facets)
{
    auto vertices = mesh->get_cone_recursive_vertices(facets);
    vertices.sort_remove_dups();
    return vertices;
}

} // namespace godzilla
