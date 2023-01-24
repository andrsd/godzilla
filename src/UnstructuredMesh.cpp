#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"
#include "IndexSet.h"
#include "petscdmplex.h"

namespace godzilla {

Parameters
UnstructuredMesh::parameters()
{
    Parameters params = Mesh::parameters();
    return params;
}

UnstructuredMesh::UnstructuredMesh(const Parameters & parameters) :
    Mesh(parameters),
    dm(nullptr),
    partitioner(nullptr),
    partition_overlap(0)
{
    _F_;
    PETSC_CHECK(PetscPartitionerCreate(get_comm(), &this->partitioner));
}

UnstructuredMesh::~UnstructuredMesh()
{
    _F_;
    PETSC_CHECK(PetscPartitionerDestroy(&this->partitioner));
    if (this->dm)
        PETSC_CHECK(DMDestroy(&this->dm));
}

DM
UnstructuredMesh::get_dm() const
{
    _F_;
    return this->dm;
}

void
UnstructuredMesh::create()
{
    _F_;
    create_dm();
    PETSC_CHECK(DMSetFromOptions(this->dm));
    PETSC_CHECK(DMViewFromOptions(dm, nullptr, "-dm_view"));
    PETSC_CHECK(DMSetUp(this->dm));
    PETSC_CHECK(DMGetDimension(this->dm, &this->dim));

    lprintf(9, "Information:");
    lprintf(9, "- vertices: %d", get_num_vertices());
    lprintf(9, "- elements: %d", get_num_elements());
}

bool
UnstructuredMesh::has_label(const std::string & name) const
{
    _F_;
    PetscBool exists = PETSC_FALSE;
    PETSC_CHECK(DMHasLabel(this->dm, name.c_str(), &exists));
    return exists == PETSC_TRUE;
}

DMLabel
UnstructuredMesh::get_label(const std::string & name) const
{
    _F_;
    DMLabel label;
    PETSC_CHECK(DMGetLabel(this->dm, name.c_str(), &label));
    return label;
}

PetscInt
UnstructuredMesh::get_num_vertices() const
{
    _F_;
    PetscInt first, last;
    get_vertex_idx_range(first, last);
    return last - first;
}

void
UnstructuredMesh::get_vertex_idx_range(PetscInt & first, PetscInt & last) const
{
    _F_;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, this->dim, &first, &last));
}

PetscInt
UnstructuredMesh::get_num_elements() const
{
    _F_;
    PetscInt first, last;
    get_element_idx_range(first, last);
    return last - first;
}

PetscInt
UnstructuredMesh::get_num_all_elements() const
{
    _F_;
    PetscInt first, last;
    get_all_element_idx_range(first, last);
    return last - first;
}

void
UnstructuredMesh::get_element_idx_range(PetscInt & first, PetscInt & last) const
{
    _F_;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &first, &last));
    PetscInt gc_first, gc_last;
    PETSC_CHECK(DMPlexGetGhostCellStratum(this->dm, &gc_first, &gc_last));
    if (gc_first != -1)
        last = gc_first;
}

void
UnstructuredMesh::get_all_element_idx_range(PetscInt & first, PetscInt & last) const
{
    _F_;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &first, &last));
}

DMPolytopeType
UnstructuredMesh::get_cell_type(PetscInt el) const
{
    DMPolytopeType polytope_type;
    PETSC_CHECK(DMPlexGetCellType(this->dm, el, &polytope_type));
    return polytope_type;
}

void
UnstructuredMesh::set_partitioner_type(const std::string & type)
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetType(this->partitioner, type.c_str()));
}

void
UnstructuredMesh::set_partition_overlap(PetscInt overlap)
{
    _F_;
    this->partition_overlap = overlap;
}

void
UnstructuredMesh::distribute()
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetUp(this->partitioner));

    PETSC_CHECK(DMPlexSetPartitioner(this->dm, this->partitioner));

    DM dm_dist = nullptr;
    PETSC_CHECK(DMPlexDistribute(this->dm, this->partition_overlap, nullptr, &dm_dist));
    if (dm_dist) {
        PETSC_CHECK(DMDestroy(&this->dm));
        this->dm = dm_dist;
    }
}

bool
UnstructuredMesh::is_simplex() const
{
    _F_;
    PetscBool simplex;
    PETSC_CHECK(DMPlexIsSimplex(this->dm, &simplex));
    return simplex == PETSC_TRUE;
}

void
UnstructuredMesh::create_face_set_labels(const std::map<PetscInt, std::string> & names)
{
    _F_;
    DMLabel fs_label = get_label("Face Sets");
    PetscInt n_fs = get_num_face_sets();
    IndexSet fs_ids = IndexSet::values_from_label(fs_label);
    fs_ids.get_indices();
    for (PetscInt i = 0; i < n_fs; i++) {
        PetscInt id = fs_ids[i];
        create_face_set(id);
        set_face_set_name(id, names.at(id));
    }
    fs_ids.restore_indices();
    fs_ids.destroy();
}

void
UnstructuredMesh::create_face_set(PetscInt id)
{
    _F_;
    DMLabel face_sets_label = get_label("Face Sets");
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(face_sets_label, id, &is));
    std::string id_str = std::to_string(id);
    PETSC_CHECK(DMCreateLabel(this->dm, id_str.c_str()));
    DMLabel label = get_label(id_str);
    if (is)
        PETSC_CHECK(DMLabelSetStratumIS(label, id, is));
    PETSC_CHECK(ISDestroy(&is));
    PETSC_CHECK(DMPlexLabelComplete(this->dm, label));
}

void
UnstructuredMesh::set_face_set_name(PetscInt id, const std::string & name)
{
    _F_;
    this->face_set_names[id] = name;
    this->face_set_ids[name] = id;
}

bool
UnstructuredMesh::has_face_set(const std::string & name) const
{
    _F_;
    const auto & it = this->face_set_ids.find(name);
    if (it != this->face_set_ids.end()) {
        PetscInt id = it->second;
        return has_label(std::to_string(id));
    }
    else
        // assume `name` is an ID
        return has_label(name);
}

DMLabel
UnstructuredMesh::get_face_set_label(const std::string & name) const
{
    _F_;
    const auto & it = this->face_set_ids.find(name);
    if (it != this->face_set_ids.end()) {
        PetscInt id = it->second;
        return get_label(std::to_string(id));
    }
    else
        // assume `name` is an ID
        return get_label(name);
}

const std::string &
UnstructuredMesh::get_face_set_name(PetscInt id) const
{
    _F_;
    const auto & it = this->face_set_names.find(id);
    if (it != this->face_set_names.end())
        return it->second;
    else
        error("Face set ID '%d' does not exist.", id);
}

void
UnstructuredMesh::create_cell_set(PetscInt id, const std::string & name)
{
    _F_;
    DMLabel cell_sets_label = get_label("Cell Sets");
    IS is;
    PETSC_CHECK(DMLabelGetStratumIS(cell_sets_label, id, &is));
    PETSC_CHECK(DMCreateLabel(this->dm, name.c_str()));
    DMLabel label = get_label(name);
    if (is)
        PETSC_CHECK(DMLabelSetStratumIS(label, id, is));
    PETSC_CHECK(ISDestroy(&is));
    PETSC_CHECK(DMPlexLabelComplete(this->dm, label));
    this->cell_set_names[id] = name;
}

const std::string &
UnstructuredMesh::get_cell_set_name(PetscInt id) const
{
    _F_;
    const auto & it = this->cell_set_names.find(id);
    if (it != this->cell_set_names.end())
        return it->second;
    else
        error("Cell set ID '%d' does not exist.", id);
}

PetscInt
UnstructuredMesh::get_num_cell_sets() const
{
    _F_;
    PetscInt n_cells_sets;
    PETSC_CHECK(DMGetLabelSize(this->dm, "Cell Sets", &n_cells_sets));
    return n_cells_sets;
}

PetscInt
UnstructuredMesh::get_num_face_sets() const
{
    PetscInt n_face_sets;
    PETSC_CHECK(DMGetLabelSize(this->dm, "Face Sets", &n_face_sets));
    return n_face_sets;
}

PetscInt
UnstructuredMesh::get_num_vertex_sets() const
{
    _F_;
    PetscInt n_vertex_sets;
    PETSC_CHECK(DMGetLabelSize(this->dm, "Vertex Sets", &n_vertex_sets));
    return n_vertex_sets;
}

void
UnstructuredMesh::construct_ghost_cells()
{
    _F_;
    DM gdm;
    PETSC_CHECK(DMPlexConstructGhostCells(this->dm, nullptr, nullptr, &gdm));
    PETSC_CHECK(DMDestroy(&this->dm));
    this->dm = gdm;
}

} // namespace godzilla
