#include "godzilla/Godzilla.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/IndexSet.h"
#include "petscdmplex.h"

namespace godzilla {

const char *
get_polytope_type_str(DMPolytopeType elem_type)
{
    _F_;
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
    partition_overlap(0),
    common_cells_by_vtx_computed(false)
{
    _F_;
    this->partitioner.create(get_comm());
}

UnstructuredMesh::~UnstructuredMesh()
{
    _F_;
    this->partitioner.destroy();
}

void
UnstructuredMesh::create()
{
    _F_;
    DM dm = create_dm();
    PETSC_CHECK(DMSetFromOptions(dm));
    PETSC_CHECK(DMViewFromOptions(dm, nullptr, "-dm_view"));
    set_dm(dm);
    set_up();

    lprint(9, "Information:");
    lprint(9, "- vertices: {}", get_num_vertices());
    lprint(9, "- elements: {}", get_num_cells());
}

Label
UnstructuredMesh::get_depth_label() const
{
    _F_;
    DMLabel depth_label;
    PETSC_CHECK(DMPlexGetDepthLabel(get_dm(), &depth_label));
    return Label(depth_label);
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
    _F_;
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), get_dimension(), &first, &last));
    return { first, last };
}

Int
UnstructuredMesh::get_num_faces() const
{
    _F_;
    return get_face_range().size();
}

UnstructuredMesh::Range
UnstructuredMesh::get_face_range() const
{
    _F_;
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 1, &first, &last));
    return { first, last };
}

Int
UnstructuredMesh::get_num_cells() const
{
    _F_;
    return get_cell_range().size();
}

Int
UnstructuredMesh::get_num_all_cells() const
{
    _F_;
    return get_all_cell_range().size();
}

UnstructuredMesh::Range
UnstructuredMesh::get_cell_range() const
{
    _F_;
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, &first, &last));
    Int gc_first, gc_last;
    PETSC_CHECK(DMPlexGetGhostCellStratum(get_dm(), &gc_first, &gc_last));
    if (gc_first != -1)
        last = gc_first;
    return { first, last };
}

UnstructuredMesh::Range
UnstructuredMesh::get_all_cell_range() const
{
    _F_;
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, &first, &last));
    return { first, last };
}

IndexSet
UnstructuredMesh::get_all_cells() const
{
    _F_;
    Int depth;
    PETSC_CHECK(DMPlexGetDepth(get_dm(), &depth));
    IS cell_is;
    PETSC_CHECK(DMGetStratumIS(get_dm(), "dim", depth, &cell_is));
    if (!cell_is)
        PETSC_CHECK(DMGetStratumIS(get_dm(), "depth", depth, &cell_is));
    return IndexSet(cell_is);
}

void
UnstructuredMesh::get_chart(Int & start, Int & end) const
{
    _F_;
    PETSC_CHECK(DMPlexGetChart(get_dm(), &start, &end));
}

UnstructuredMesh::Range
UnstructuredMesh::get_chart() const
{
    _F_;
    Int start, end;
    PETSC_CHECK(DMPlexGetChart(get_dm(), &start, &end));
    return Range(start, end);
}

DMPolytopeType
UnstructuredMesh::get_cell_type(Int el) const
{
    _F_;
    DMPolytopeType polytope_type;
    PETSC_CHECK(DMPlexGetCellType(get_dm(), el, &polytope_type));
    return polytope_type;
}

std::vector<Int>
UnstructuredMesh::get_connectivity(Int point) const
{
    _F_;
    Int closure_size;
    Int * closure = nullptr;
    PETSC_CHECK(DMPlexGetTransitiveClosure(get_dm(), point, PETSC_TRUE, &closure_size, &closure));

    auto polytope_type = get_cell_type(point);
    Int n_elem_nodes = UnstructuredMesh::get_num_cell_nodes(polytope_type);
    std::vector<Int> elem_connect;
    elem_connect.resize(n_elem_nodes);
    for (Int k = 0; k < n_elem_nodes; k++) {
        Int l = 2 * (closure_size - n_elem_nodes + k);
        elem_connect[k] = closure[l];
    }

    PETSC_CHECK(
        DMPlexRestoreTransitiveClosure(get_dm(), point, PETSC_TRUE, &closure_size, &closure));

    return elem_connect;
}

std::vector<Int>
UnstructuredMesh::get_support(Int point) const
{
    _F_;
    Int n_support;
    PETSC_CHECK(DMPlexGetSupportSize(get_dm(), point, &n_support));
    const Int * support;
    PETSC_CHECK(DMPlexGetSupport(get_dm(), point, &support));
    std::vector<Int> v;
    v.resize(n_support);
    for (Int i = 0; i < n_support; i++)
        v[i] = support[i];
    return v;
}

std::vector<Int>
UnstructuredMesh::get_cone(Int point) const
{
    _F_;
    Int n;
    PETSC_CHECK(DMPlexGetConeSize(get_dm(), point, &n));
    const Int * cone;
    PETSC_CHECK(DMPlexGetCone(get_dm(), point, &cone));
    std::vector<Int> v;
    v.resize(n);
    for (Int i = 0; i < n; i++)
        v[i] = cone[i];
    return v;
}

IndexSet
UnstructuredMesh::get_cone_recursive_vertices(IndexSet points) const
{
    _F_;
    IndexSet expanded_points;
    PETSC_CHECK(DMPlexGetConeRecursiveVertices(get_dm(), points, expanded_points));
    return expanded_points;
}

void
UnstructuredMesh::set_partitioner_type(const std::string & type)
{
    _F_;
    this->partitioner.set_type(type);
}

Int
UnstructuredMesh::get_partition_overlap()
{
    _F_;
    return this->partition_overlap;
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
    TIMED_EVENT(9, "MeshDistribution", "Distributing");
    this->partitioner.set_up();

    PETSC_CHECK(DMPlexSetPartitioner(get_dm(), this->partitioner));

    DM dm_dist = nullptr;
    PETSC_CHECK(DMPlexDistribute(get_dm(), this->partition_overlap, nullptr, &dm_dist));
    if (dm_dist)
        set_dm(dm_dist);
}

bool
UnstructuredMesh::is_simplex() const
{
    _F_;
    PetscBool simplex;
    PETSC_CHECK(DMPlexIsSimplex(get_dm(), &simplex));
    return simplex == PETSC_TRUE;
}

void
UnstructuredMesh::create_face_set_labels(const std::map<Int, std::string> & names)
{
    _F_;
    Label fs_label = get_label("Face Sets");
    if (fs_label) {
        Int n_fs = fs_label.get_num_values();
        IndexSet fs_ids = fs_label.get_value_index_set();
        fs_ids.get_indices();
        for (Int i = 0; i < n_fs; i++) {
            Int id = fs_ids[i];
            create_face_set(id, names.at(id));
        }
        fs_ids.restore_indices();
        fs_ids.destroy();
    }
}

void
UnstructuredMesh::create_face_set(Int id, const std::string & name)
{
    _F_;
    Label face_sets_label = get_label("Face Sets");
    IndexSet is = face_sets_label.get_stratum(id);
    if (!is.empty()) {
        Label label = create_label(name);
        label.set_stratum(id, is);
    }
    is.destroy();
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
    return it != this->face_set_ids.end();
}

Label
UnstructuredMesh::get_face_set_label(const std::string & name) const
{
    _F_;
    const auto & it = this->face_set_ids.find(name);
    if (it != this->face_set_ids.end())
        return get_label(name);
    else
        return Label();
}

const std::string &
UnstructuredMesh::get_face_set_name(Int id) const
{
    _F_;
    const auto & it = this->face_set_names.find(id);
    if (it != this->face_set_names.end())
        return it->second;
    else
        error("Face set ID '{}' does not exist.", id);
}

void
UnstructuredMesh::create_cell_set(Int id, const std::string & name)
{
    _F_;
    auto cell_sets_label = get_label("Cell Sets");
    auto cell_set = cell_sets_label.get_stratum(id);
    auto label = create_label(name);
    if (!cell_set.empty())
        label.set_stratum(id, cell_set);
    cell_set.destroy();
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
        error("Cell set ID '{}' does not exist.", id);
}

Int
UnstructuredMesh::get_cell_set_id(const std::string & name) const
{
    _F_;
    const auto & it = this->cell_set_ids.find(name);
    if (it != this->cell_set_ids.end())
        return it->second;
    else
        error("Cell set '{}' does not exist.", name);
}

Int
UnstructuredMesh::get_num_cell_sets() const
{
    _F_;
    Int n_cells_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Cell Sets", &n_cells_sets));
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
    _F_;
    Int n_face_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Face Sets", &n_face_sets));
    return n_face_sets;
}

Int
UnstructuredMesh::get_num_vertex_sets() const
{
    _F_;
    Int n_vertex_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Vertex Sets", &n_vertex_sets));
    return n_vertex_sets;
}

void
UnstructuredMesh::construct_ghost_cells()
{
    _F_;
    DM gdm;
    PETSC_CHECK(DMPlexConstructGhostCells(get_dm(), nullptr, nullptr, &gdm));
    set_dm(gdm);
}

void
UnstructuredMesh::compute_cell_geometry(Int cell, Real * vol, Real centroid[], Real normal[]) const
{
    _F_;
    PETSC_CHECK(DMPlexComputeCellGeometryFVM(get_dm(), cell, vol, centroid, normal));
}

int
UnstructuredMesh::get_num_cell_nodes(DMPolytopeType elem_type)
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
        error("Unsupported type '{}'.", get_polytope_type_str(elem_type));
    }
}

UnstructuredMesh::Iterator
UnstructuredMesh::vertex_begin() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), get_dimension(), &idx, nullptr));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::vertex_end() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), get_dimension(), nullptr, &idx));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::face_begin() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 1, &idx, nullptr));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::face_end() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 1, nullptr, &idx));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::cell_begin() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, &idx, nullptr));
    return Iterator(idx);
}

UnstructuredMesh::Iterator
UnstructuredMesh::cell_end() const
{
    _F_;
    Int idx;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, nullptr, &idx));
    return Iterator(idx);
}

const std::map<Int, std::vector<Int>> &
UnstructuredMesh::common_cells_by_vertex()
{
    _F_;
    if (!this->common_cells_by_vtx_computed) {
        for (auto & cell : get_cell_range()) {
            auto connect = get_connectivity(cell);
            for (auto & vtx : connect)
                this->common_cells_by_vtx[vtx].push_back(cell);
        }
        this->common_cells_by_vtx_computed = true;
    }
    return this->common_cells_by_vtx;
}

DM
UnstructuredMesh::build_from_cell_list(Int dim,
                                       Int n_corners,
                                       const std::vector<Int> & cells,
                                       Int space_dim,
                                       const std::vector<Real> & vertices,
                                       bool interpolate)
{
    _F_;
    DM dm;
    PETSC_CHECK(DMPlexCreateFromCellListPetsc(get_comm(),
                                              dim,
                                              cells.size() / n_corners,
                                              vertices.size() / space_dim,
                                              n_corners,
                                              interpolate ? PETSC_TRUE : PETSC_FALSE,
                                              cells.data(),
                                              space_dim,
                                              vertices.data(),
                                              &dm));
    return dm;
}

} // namespace godzilla
