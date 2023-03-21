#include "Godzilla.h"
#include "UnstructuredMesh.h"
#include "CallStack.h"
#include "IndexSet.h"
#include "petscdmplex.h"

namespace godzilla {

const char *
get_polytope_type_str(DMPolytopeType elem_type)
{
    switch (elem_type) {
    case DM_POLYTOPE_POINT:
        return "POINT";
    case DM_POLYTOPE_SEGMENT:
        return "SEGMENT";
    case DM_POLYTOPE_POINT_PRISM_TENSOR:
        return "POINT_PRISM_TENSOR";
    case DM_POLYTOPE_TRIANGLE:
        return "TRIANGLE";
    case DM_POLYTOPE_QUADRILATERAL:
        return "QUADRILATERAL";
    case DM_POLYTOPE_SEG_PRISM_TENSOR:
        return "SEG_PRISM_TENSOR";
    case DM_POLYTOPE_TETRAHEDRON:
        return "TETRAHEDRON";
    case DM_POLYTOPE_HEXAHEDRON:
        return "HEXAHEDRON";
    case DM_POLYTOPE_TRI_PRISM:
        return "TRI_PRISM";
    case DM_POLYTOPE_TRI_PRISM_TENSOR:
        return "TRI_PRISM_TENSOR";
    case DM_POLYTOPE_QUAD_PRISM_TENSOR:
        return "QUAD_PRISM_TENSOR";
    case DM_POLYTOPE_PYRAMID:
        return "PYRAMID";
    case DM_POLYTOPE_FV_GHOST:
        return "FV_GHOST";
    case DM_POLYTOPE_INTERIOR_GHOST:
        return "INTERIOR_GHOST";
    default:
        return "UNKNOWN";
    }
}

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

Vector
UnstructuredMesh::get_coordinates() const
{
    Vec vec;
    PETSC_CHECK(DMGetCoordinates(this->dm, &vec));
    return { vec };
}

Vector
UnstructuredMesh::get_coordinates_local() const
{
    Vec vec;
    PETSC_CHECK(DMGetCoordinatesLocal(this->dm, &vec));
    return { vec };
}

Int
UnstructuredMesh::get_num_vertices() const
{
    _F_;
    return get_vertex_range().size();
}

UnstructuredMesh::Range
UnstructuredMesh::get_vertex_range() const
{
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, this->dim, &first, &last));
    return { first, last };
}

Int
UnstructuredMesh::get_num_elements() const
{
    _F_;
    return get_element_range().size();
}

Int
UnstructuredMesh::get_num_all_elements() const
{
    _F_;
    return get_all_element_range().size();
}

UnstructuredMesh::Range
UnstructuredMesh::get_element_range() const
{
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &first, &last));
    Int gc_first, gc_last;
    PETSC_CHECK(DMPlexGetGhostCellStratum(this->dm, &gc_first, &gc_last));
    if (gc_first != -1)
        last = gc_first;
    return { first, last };
}

UnstructuredMesh::Range
UnstructuredMesh::get_all_element_range() const
{
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &first, &last));
    return { first, last };
}

IndexSet
UnstructuredMesh::get_all_elements() const
{
    Int depth;
    PETSC_CHECK(DMPlexGetDepth(this->dm, &depth));
    IS cell_is;
    PETSC_CHECK(DMGetStratumIS(this->dm, "dim", depth, &cell_is));
    if (!cell_is)
        PETSC_CHECK(DMGetStratumIS(this->dm, "depth", depth, &cell_is));
    return IndexSet(cell_is);
}

DMPolytopeType
UnstructuredMesh::get_cell_type(Int el) const
{
    DMPolytopeType polytope_type;
    PETSC_CHECK(DMPlexGetCellType(this->dm, el, &polytope_type));
    return polytope_type;
}

std::vector<Int>
UnstructuredMesh::get_cell_connectivity(Int cell_id) const
{
    Int n_all_elems = get_num_all_elements();

    Int closure_size;
    Int * closure = nullptr;
    PETSC_CHECK(DMPlexGetTransitiveClosure(this->dm, cell_id, PETSC_TRUE, &closure_size, &closure));

    auto polytope_type = get_cell_type(cell_id);
    Int n_elem_nodes = UnstructuredMesh::get_num_elem_nodes(polytope_type);
    std::vector<Int> elem_connect;
    elem_connect.resize(n_elem_nodes);
    for (Int k = 0; k < n_elem_nodes; k++) {
        Int l = 2 * (closure_size - n_elem_nodes + k);
        Int idx = (closure[l] - n_all_elems);
        elem_connect[k] = idx;
    }

    PETSC_CHECK(
        DMPlexRestoreTransitiveClosure(this->dm, cell_id, PETSC_TRUE, &closure_size, &closure));

    return elem_connect;
}

std::vector<Int>
UnstructuredMesh::get_support(Int point) const
{
    _F_;
    Int n_support;
    PETSC_CHECK(DMPlexGetSupportSize(this->dm, point, &n_support));
    const Int * support;
    PETSC_CHECK(DMPlexGetSupport(this->dm, point, &support));
    std::vector<Int> v;
    v.resize(n_support);
    for (Int i = 0; i < n_support; i++)
        v[i] = support[i];
    return v;
}

void
UnstructuredMesh::set_partitioner_type(const std::string & type)
{
    _F_;
    PETSC_CHECK(PetscPartitionerSetType(this->partitioner, type.c_str()));
}

void
UnstructuredMesh::set_partition_overlap(Int overlap)
{
    _F_;
    this->partition_overlap = overlap;
}

void
UnstructuredMesh::distribute()
{
    _F_;
    lprintf(9, "Distributing");
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
UnstructuredMesh::create_face_set_labels(const std::map<Int, std::string> & names)
{
    _F_;
    DMLabel fs_label = get_label("Face Sets");
    if (fs_label) {
        Int n_fs;
        DMLabelGetNumValues(fs_label, &n_fs);
        IndexSet fs_ids = IndexSet::values_from_label(fs_label);
        fs_ids.get_indices();
        for (Int i = 0; i < n_fs; i++) {
            Int id = fs_ids[i];
            create_face_set(id);
        }
        fs_ids.restore_indices();
        fs_ids.destroy();

        for (auto & it : names)
            set_face_set_name(it.first, it.second);
    }
}

void
UnstructuredMesh::create_face_set(Int id)
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
UnstructuredMesh::set_face_set_name(Int id, const std::string & name)
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
        Int id = it->second;
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
        Int id = it->second;
        return get_label(std::to_string(id));
    }
    else
        // assume `name` is an ID
        return get_label(name);
}

const std::string &
UnstructuredMesh::get_face_set_name(Int id) const
{
    _F_;
    const auto & it = this->face_set_names.find(id);
    if (it != this->face_set_names.end())
        return it->second;
    else
        error("Face set ID '%d' does not exist.", id);
}

void
UnstructuredMesh::create_cell_set(Int id, const std::string & name)
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
    this->cell_set_ids[name] = id;
}

const std::string &
UnstructuredMesh::get_cell_set_name(Int id) const
{
    _F_;
    const auto & it = this->cell_set_names.find(id);
    if (it != this->cell_set_names.end())
        return it->second;
    else
        error("Cell set ID '%d' does not exist.", id);
}

Int
UnstructuredMesh::get_cell_set_id(const std::string & name) const
{
    _F_;
    const auto & it = this->cell_set_ids.find(name);
    if (it != this->cell_set_ids.end())
        return it->second;
    else
        error("Cell set '%s' does not exist.", name);
}

Int
UnstructuredMesh::get_num_cell_sets() const
{
    _F_;
    Int n_cells_sets;
    PETSC_CHECK(DMGetLabelSize(this->dm, "Cell Sets", &n_cells_sets));
    return n_cells_sets;
}

const std::map<Int, std::string> &
UnstructuredMesh::get_cell_sets() const
{
    _F_;
    return this->cell_set_names;
}

Int
UnstructuredMesh::get_num_face_sets() const
{
    Int n_face_sets;
    PETSC_CHECK(DMGetLabelSize(this->dm, "Face Sets", &n_face_sets));
    return n_face_sets;
}

Int
UnstructuredMesh::get_num_vertex_sets() const
{
    _F_;
    Int n_vertex_sets;
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

Section
UnstructuredMesh::get_local_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetLocalSection(this->dm, &section));
    return { section };
}

Section
UnstructuredMesh::get_global_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetGlobalSection(this->dm, &section));
    return { section };
}

int
UnstructuredMesh::get_num_elem_nodes(DMPolytopeType elem_type)
{
    _F_;
    switch (elem_type) {
    case DM_POLYTOPE_POINT:
        return 1;
    case DM_POLYTOPE_SEGMENT:
        return 2;
    case DM_POLYTOPE_TRIANGLE:
        return 3;
    case DM_POLYTOPE_QUADRILATERAL:
    case DM_POLYTOPE_TETRAHEDRON:
        return 4;
    case DM_POLYTOPE_HEXAHEDRON:
        return 8;
    default:
        error("Unsupported type '%s'.", get_polytope_type_str(elem_type));
    }
}

UnstructuredMesh::Iterator
UnstructuredMesh::vertex_begin() const
{
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, this->dim, &idx, nullptr));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::vertex_end() const
{
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, this->dim, nullptr, &idx));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::cell_begin() const
{
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, &idx, nullptr));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::cell_end() const
{
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(this->dm, 0, nullptr, &idx));
    return Iterator(idx);
}

} // namespace godzilla
