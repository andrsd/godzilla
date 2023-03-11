#pragma once

#include "GodzillaConfig.h"
#include "Mesh.h"
#include "IndexSet.h"
#include "Vector.h"
#include "petscpartitioner.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Mesh {
public:
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
    /// @return DMLabel associated with the `name`
    NO_DISCARD virtual DMLabel get_label(const std::string & name) const;

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
    /// @param first First vertex index
    /// @param last Last vertex index plus one
    void get_vertex_idx_range(Int & first, Int & last) const;

    /// Return the number of mesh elements (interior)
    NO_DISCARD virtual Int get_num_elements() const;

    /// Return the number of all mesh elements (interior + ghosted)
    NO_DISCARD virtual Int get_num_all_elements() const;

    /// Get range of element indices (interior only)
    ///
    /// @param first First element index
    /// @param last Last element index plus one
    void get_element_idx_range(Int & first, Int & last) const;

    /// Get range of all element indices (interior + ghosted)
    ///
    /// @param first First element index
    /// @param last Last element index plus one
    void get_all_element_idx_range(Int & first, Int & last) const;

    /// Get index set with all elements
    ///
    /// @return Index set with all elements
    IndexSet get_all_elements() const;

    /// Get cell type
    ///
    /// @param el Element index
    /// @return Cell type
    NO_DISCARD virtual DMPolytopeType get_cell_type(Int el) const;

    /// Get connectivity
    ///
    /// @param cell_id ID of the cell
    /// @return Cell connectivity
    std::vector<Int> get_cell_connectivity(Int cell_id) const;

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
    /// @return DMLabel associated with face set name
    NO_DISCARD DMLabel get_face_set_label(const std::string & name) const;

    /// Get number of vertex sets
    ///
    /// @return Number of vertex sets
    NO_DISCARD Int get_num_vertex_sets() const;

    void distribute() override;

    /// Construct ghost cells which connect to every boundary face
    ///
    virtual void construct_ghost_cells();

protected:
    /// Method that builds DM for the mesh
    virtual void create_dm() = 0;

    void create_cell_set(Int id, const std::string & name);

    void create_face_set_labels(const std::map<Int, std::string> & names);

    void create_face_set(Int id);

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

    static int get_num_elem_nodes(DMPolytopeType elem_type);
};

} // namespace godzilla
