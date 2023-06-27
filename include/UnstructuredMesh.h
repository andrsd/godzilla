#pragma once

#include "GodzillaConfig.h"
#include "Mesh.h"
#include "IndexSet.h"
#include "Vector.h"
#include "Section.h"
#include "Label.h"
#include "petscpartitioner.h"

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

    Iterator cell_begin() const;
    Iterator cell_end() const;

    /// Contiguous range of indices
    struct Range {
        Range() : first(-1), last(-1) {}
        Range(Int first, Int last) : first(first), last(last) {}

        Iterator
        begin() const
        {
            return Iterator(this->first);
        }

        Iterator
        end() const
        {
            return Iterator(this->last);
        }

        /// Get the number of indices in the range
        Int
        size() const
        {
            return last - first;
        }

        Int
        get_first() const
        {
            return first;
        }

        Int
        get_last() const
        {
            return last;
        }

    private:
        /// First index
        Int first;
        /// Last index (not included in the range)
        Int last;
    };

    //

    explicit UnstructuredMesh(const Parameters & parameters);
    ~UnstructuredMesh() override;

    NO_DISCARD DM get_dm() const override;
    void create() override;

    /// Check if mesh has label with a name
    ///
    /// @param name The name of the label
    /// @return true if label exists, otherwise false
    NO_DISCARD virtual bool has_label(const std::string & name) const;

    /// Get label associated with a name
    ///
    /// @param name Label name
    /// @return Label associated with the `name`
    NO_DISCARD virtual Label get_label(const std::string & name) const;

    /// Get the `Label` recording the depth of each point
    ///
    /// @return The `Label` recording point depth
    Label get_depth_label() const;

    /// Create label
    ///
    /// @param name Label name
    /// @return Created label
    Label create_label(const std::string & name) const;

    /// Gets the DM that prescribes coordinate layout and scatters between global and local
    /// coordinates
    ///
    /// @return coordinate `DM`
    DM get_coordinate_dm() const;

    /// Get a global vector with the coordinates associated with this mesh
    ///
    /// @return Global coordinate vector
    Vector get_coordinates() const;

    /// Get a local vector with the coordinates associated with this mesh
    ///
    /// @return Coordinate vector
    Vector get_coordinates_local() const;

    /// Return the number of mesh vertices
    NO_DISCARD virtual Int get_num_vertices() const;

    /// Get range of vertex indices
    ///
    /// @return Range of vertex indices
    Range get_vertex_range() const;

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

    /// Get cell type
    ///
    /// @param cell Cell index
    /// @return Cell type
    NO_DISCARD virtual DMPolytopeType get_cell_type(Int cell) const;

    /// Get connectivity
    ///
    /// @param cell_id ID of the cell
    /// @return Cell connectivity
    std::vector<Int> get_cell_connectivity(Int cell_id) const;

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

    /// Get the Section encoding the local data layout for the DM
    Section get_local_section() const;

    /// Get the Section encoding the global data layout for the DM
    Section get_global_section() const;

protected:
    /// Method that builds DM for the mesh
    virtual void create_dm() = 0;

    void create_cell_set(Int id, const std::string & name);

    void create_face_set_labels(const std::map<Int, std::string> & names);

    void create_face_set(Int id, const std::string & name);

    /// DM object
    DM dm;

    /// Mesh partitioner
    PetscPartitioner partitioner;

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
