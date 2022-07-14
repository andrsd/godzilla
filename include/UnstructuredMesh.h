#pragma once

#include "Mesh.h"
#include "petscpartitioner.h"

namespace godzilla {

/// Base class for unstructured meshes
///
class UnstructuredMesh : public Mesh {
public:
    UnstructuredMesh(const Parameters & parameters);
    virtual ~UnstructuredMesh();

    virtual void create() override;

    /// Return the number of mesh vertices
    virtual PetscInt get_num_vertices() const;

    /// Get range of vertex indices
    ///
    /// @param first First vertex index
    /// @param last Last vertex index plus one
    void get_vertex_idx_range(PetscInt & first, PetscInt & last) const;

    /// Return the number of mesh elements (interior)
    virtual PetscInt get_num_elements() const;

    /// Return the number of all mesh elements (interior + ghosted)
    virtual PetscInt get_num_all_elements() const;

    /// Get range of element indices (interior only)
    ///
    /// @param first First element index
    /// @param last Last element index plus one
    void get_element_idx_range(PetscInt & first, PetscInt & last) const;

    /// Get range of all element indices (interior + ghosted)
    ///
    /// @param first First element index
    /// @param last Last element index plus one
    void get_all_element_idx_range(PetscInt & first, PetscInt & last) const;

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partitioner_type(const std::string & type);

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    virtual void set_partition_overlap(PetscInt overlap);

    /// Is the first cell in the mesh a simplex?
    ///
    /// @return true if cell is a simplex, otherwise false
    virtual bool is_simplex() const;

    /// Get cell set name
    ///
    /// @param id The ID of the cell set
    /// @return Cell set name
    const std::string & get_cell_set_name(PetscInt id) const;

    /// Get number of cell sets
    ///
    /// @return Number of cell sets
    PetscInt get_num_cell_sets() const;

    /// Get face set name
    ///
    /// @param id The ID of the face set
    /// @return Facet name
    const std::string & get_face_set_name(PetscInt id) const;

    /// Set face set name
    ///
    /// @param id The ID of the face set
    /// @param name The name of the face set
    void set_face_set_name(PetscInt id, const std::string & name);

    /// Check if mesh has a label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return true if label exists, false otherwise
    bool has_face_set(const std::string & name) const;

    /// Get label corresponding to a face set name
    ///
    /// @param name The name of the face set
    /// @return DMLabel associated with face set name
    DMLabel get_face_set_label(const std::string & name) const;

    /// Get number of vertex sets
    ///
    /// @return Number of vertex sets
    PetscInt get_num_vertex_sets() const;

    virtual void distribute() override;

    /// Construct ghost cells which connect to every boundary face
    ///
    virtual void construct_ghost_cells();

protected:
    void create_cell_set(PetscInt id, const std::string & name);

    void create_face_set_labels(const std::map<int, std::string> & names);

    void create_face_set(PetscInt id);

    /// Mesh partitioner
    PetscPartitioner partitioner;

    /// Partition overlap for mesh partitioning
    PetscInt partition_overlap;

    /// Cell set names
    std::map<PetscInt, std::string> cell_set_names;

    /// Face set names
    std::map<PetscInt, std::string> face_set_names;

    /// Face set IDs
    std::map<std::string, PetscInt> face_set_ids;

public:
    static Parameters parameters();
};

} // namespace godzilla
