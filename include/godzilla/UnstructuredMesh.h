#pragma once

#include "godzilla/GodzillaConfig.h"
#include "godzilla/Mesh.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Vector.h"
#include "godzilla/Section.h"
#include "godzilla/Partitioner.h"
#include "godzilla/DenseVector.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Mesh {
public:
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Int;

        explicit Iterator(Int idx) : idx(idx) {}

        const value_type &
        operator*() const
        {
            return this->idx;
        }

        /// Prefix increment
        Iterator &
        operator++()
        {
            this->idx++;
            return *this;
        }

        /// Postfix increment
        Iterator
        operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool
        operator==(const Iterator & a, const Iterator & b)
        {
            return a.idx == b.idx;
        };

        friend bool
        operator!=(const Iterator & a, const Iterator & b)
        {
            return a.idx != b.idx;
        };

    private:
        Int idx;
    };

    Iterator vertex_begin() const;
    Iterator vertex_end() const;

    Iterator face_begin() const;
    Iterator face_end() const;

    Iterator cell_begin() const;
    Iterator cell_end() const;

    /// Contiguous range of indices
    struct Range {
        Range() : first_idx(-1), last_idx(-1) {}
        Range(Int first, Int last) : first_idx(first), last_idx(last) {}

        Iterator
        begin() const
        {
            return Iterator(this->first_idx);
        }

        Iterator
        end() const
        {
            return Iterator(this->last_idx);
        }

        /// Get the number of indices in the range
        Int
        size() const
        {
            return last_idx - first_idx;
        }

        /// Get the first index in the range
        ///
        /// @return First index in the range
        [[deprecated("Use first() instead")]] Int
        get_first() const
        {
            return first_idx;
        }

        /// Get the first index in the range
        ///
        /// @return First index in the range
        Int
        first() const
        {
            return first_idx;
        }

        /// Get the last index (not included) in the range
        ///
        /// @return Last index (not included) in the range
        [[deprecated("Use last() instead")]] Int
        get_last() const
        {
            return last_idx;
        }

        /// Get the last index (not included) in the range
        ///
        /// @return Last index (not included) in the range
        Int
        last() const
        {
            return last_idx;
        }

    private:
        /// First index
        Int first_idx;
        /// Last index (not included in the range)
        Int last_idx;
    };

    //

    explicit UnstructuredMesh(const Parameters & parameters);
    ~UnstructuredMesh() override;

    void create() override;

    /// Build from a list of vertices for each cell (common mesh generator output)
    ///
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
    DM build_from_cell_list(Int dim,
                            Int n_corners,
                            const std::vector<Int> & cells,
                            Int space_dim,
                            const std::vector<Real> & vertices,
                            bool interpolate);

    /// Get the `Label` recording the depth of each point
    ///
    /// @return The `Label` recording point depth
    Label get_depth_label() const;

    /// Return the number of mesh vertices
    NO_DISCARD virtual Int get_num_vertices() const;

    /// Get range of vertex indices
    ///
    /// @return Range of vertex indices
    Range get_vertex_range() const;

    /// Return the number of mesh faces
    NO_DISCARD virtual Int get_num_faces() const;

    /// Get range of face indices
    ///
    /// @return Range of face indices
    Range get_face_range() const;

    /// Return the number of mesh cells (interior)
    ///
    /// @return Number of mesh cells (interior)
    NO_DISCARD virtual Int get_num_cells() const;

    /// Return the number of all mesh cells (interior + ghosted)
    ///
    /// @return Number of all mesh cells (interior + ghosted)
    NO_DISCARD virtual Int get_num_all_cells() const;

    /// Get range of cell indices (interior only)
    ///
    /// @return Range of cell indices
    Range get_cell_range() const;

    /// Get range of all cell indices (interior + ghosted)
    ///
    /// @param Range of all cell indices (interior + ghosted)
    Range get_all_cell_range() const;

    /// Get index set with all cells
    ///
    /// @return Index set with all cells
    IndexSet get_all_cells() const;

    /// Return the interval for all mesh points [start, end)
    ///
    /// @param start The first mesh point
    /// @param end The upper bound for mesh points
    void get_chart(Int & start, Int & end) const;

    /// Return the interval for all mesh points [start, end)
    ///
    /// @return Range of mesh points
    Range get_chart() const;

    /// Get cell type
    ///
    /// @param cell Cell index
    /// @return Cell type
    NO_DISCARD virtual DMPolytopeType get_cell_type(Int cell) const;

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

    /// Return the points on the in-edges for this point
    ///
    /// @param point Point with must lie in the chart
    /// @return Points which are on the out-edges for point `p`
    std::vector<Int> get_cone(Int point) const;

    /// Expand each given point into its cone points and do that recursively until we end up just
    /// with vertices.
    ///
    /// @param points IndexSet of points with must lie in the chart
    /// @return Vertices recursively expanded from input points
    IndexSet get_cone_recursive_vertices(IndexSet points) const;

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Get partition overlap
    ///
    /// @return Partition overlap
    Int get_partition_overlap();

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(Int overlap);

    /// Is the first cell in the mesh a simplex?
    ///
    /// @return true if cell is a simplex, otherwise false
    NO_DISCARD virtual bool is_simplex() const;

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
    NO_DISCARD Int get_num_cell_sets() const;

    /// Get cell sets
    ///
    /// @return Cell sets
    const std::map<Int, std::string> & get_cell_sets() const;

    /// Get face set name
    ///
    /// @param id The ID of the face set
    /// @return Facet name
    const std::string & get_face_set_name(Int id) const;

    /// Get number of face sets
    ///
    /// @return Number of face sets
    NO_DISCARD Int get_num_face_sets() const;

    /// Set face set name
    ///
    /// @param id The ID of the face set
    /// @param name The name of the face set
    void set_face_set_name(Int id, const std::string & name);

    /// Check if mesh has a label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return true if label exists, false otherwise
    NO_DISCARD bool has_face_set(const std::string & name) const;

    /// Get label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return Label associated with face set name
    NO_DISCARD Label get_face_set_label(const std::string & name) const;

    /// Get number of vertex sets
    ///
    /// @return Number of vertex sets
    NO_DISCARD Int get_num_vertex_sets() const;

    void distribute() override;

    /// Construct ghost cells which connect to every boundary face
    ///
    virtual void construct_ghost_cells();

    /// Compute the volume for a given cell
    ///
    /// @param cell The cell
    /// @param vol The cell volume
    /// @param centroid The cell centroid
    /// @param normal The cell normal, if appropriate
    void compute_cell_geometry(Int cell, Real * vol, Real centroid[], Real normal[]) const;

    /// Compute a map of cells common to a vertex
    ///
    /// @return A mapping [vertex index -> list of cell indices]
    const std::map<Int, std::vector<Int>> & common_cells_by_vertex();

protected:
    /// Method that builds DM for the mesh
    virtual DM create_dm() = 0;

    void create_cell_set(Int id, const std::string & name);

    void create_face_set_labels(const std::map<Int, std::string> & names);

    void create_face_set(Int id, const std::string & name);

private:
    /// Mesh partitioner
    Partitioner partitioner;

    /// Partition overlap for mesh partitioning
    Int partition_overlap;

    /// Cell set names
    std::map<Int, std::string> cell_set_names;

    /// Cell set IDs
    std::map<std::string, Int> cell_set_ids;

    /// Face set names
    std::map<Int, std::string> face_set_names;

    /// Face set IDs
    std::map<std::string, Int> face_set_ids;

    /// Cells common to a vertex
    std::map<Int, std::vector<Int>> common_cells_by_vtx;
    /// Flag indicating if `common_cells_by_vtx` was computed
    bool common_cells_by_vtx_computed;

public:
    static Parameters parameters();

    static int get_num_cell_nodes(DMPolytopeType cell_type);
};

/// Get string representation of a polytope type
///
/// @param cell_type Cell type
/// @return String describing the polytope type
const char * get_polytope_type_str(DMPolytopeType cell_type);

} // namespace godzilla