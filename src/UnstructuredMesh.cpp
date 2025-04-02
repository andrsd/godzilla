// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/UnstructuredMesh.h"
#include "godzilla/CallStack.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Exception.h"
#include "godzilla/Partitioner.h"
#include "godzilla/Convert.h"
#include "petscdmplex.h"
#include "petscdmtypes.h"

namespace godzilla {

UnstructuredMesh::UnstructuredMesh(const mpi::Communicator & comm) :
    Mesh(nullptr),
    common_cells_by_vtx_computed(false)
{
    DM dm;
    PETSC_CHECK(DMCreate(comm, &dm));
    PETSC_CHECK(DMSetType(dm, DMPLEX));
    set_dm(dm);
}

UnstructuredMesh::UnstructuredMesh(DM dm) : Mesh(dm), common_cells_by_vtx_computed(false)
{
    CALL_STACK_MSG();
}

UnstructuredMesh::~UnstructuredMesh()
{
    CALL_STACK_MSG();
}

UnstructuredMesh
UnstructuredMesh::clone() const
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMClone(get_dm(), &dm));
    return UnstructuredMesh(dm);
}

Label
UnstructuredMesh::get_depth_label() const
{
    CALL_STACK_MSG();
    DMLabel depth_label;
    PETSC_CHECK(DMPlexGetDepthLabel(get_dm(), &depth_label));
    return Label(depth_label);
}

Int
UnstructuredMesh::get_num_vertices() const
{
    CALL_STACK_MSG();
    return get_vertex_range().size();
}

Range
UnstructuredMesh::get_vertex_range() const
{
    CALL_STACK_MSG();
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), get_dimension(), &first, &last));
    return { first, last };
}

Int
UnstructuredMesh::get_num_faces() const
{
    CALL_STACK_MSG();
    return get_face_range().size();
}

Range
UnstructuredMesh::get_face_range() const
{
    CALL_STACK_MSG();
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 1, &first, &last));
    return { first, last };
}

Int
UnstructuredMesh::get_num_cells() const
{
    CALL_STACK_MSG();
    return get_cell_range().size();
}

Int
UnstructuredMesh::get_num_all_cells() const
{
    CALL_STACK_MSG();
    return get_all_cell_range().size();
}

Range
UnstructuredMesh::get_cell_range() const
{
    CALL_STACK_MSG();
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, &first, &last));
    Int gc_first, gc_last;
#if PETSC_VERSION_GE(3, 20, 0)
    PETSC_CHECK(DMPlexGetCellTypeStratum(get_dm(), DM_POLYTOPE_FV_GHOST, &gc_first, &gc_last));
#else
    PETSC_CHECK(DMPlexGetGhostCellStratum(get_dm(), &gc_first, &gc_last));
#endif
    if (gc_first != -1)
        last = gc_first;
    return { first, last };
}

Range
UnstructuredMesh::get_all_cell_range() const
{
    CALL_STACK_MSG();
    Int first, last;
    PETSC_CHECK(DMPlexGetHeightStratum(get_dm(), 0, &first, &last));
    return { first, last };
}

Range
UnstructuredMesh::get_my_cell_range() const
{
    CALL_STACK_MSG();
    // Note: this works becuase with overlap > 1, the cells owned by a process are numbered
    // consecutively, and cells that are in the overlap follows (also with consecutive numbering).
    auto my_cells = get_label("my_cells");
    auto [first, last] = my_cells.get_stratum_bounds(1);
    return { first, last };
}

Range
UnstructuredMesh::get_ghost_cell_range() const
{
    CALL_STACK_MSG();
    Int first, last;
#if PETSC_VERSION_GE(3, 20, 0)
    PETSC_CHECK(DMPlexGetCellTypeStratum(get_dm(), DM_POLYTOPE_FV_GHOST, &first, &last));
#else
    PETSC_CHECK(DMPlexGetGhostCellStratum(get_dm(), &first, &last));
#endif
    return { first, last };
}

IndexSet
UnstructuredMesh::get_all_cells() const
{
    CALL_STACK_MSG();
    Int depth;
    PETSC_CHECK(DMPlexGetDepth(get_dm(), &depth));
    IS cell_is;
    PETSC_CHECK(DMGetStratumIS(get_dm(), "dim", depth, &cell_is));
    if (!cell_is)
        PETSC_CHECK(DMGetStratumIS(get_dm(), "depth", depth, &cell_is));
    return IndexSet(cell_is);
}

IndexSet
UnstructuredMesh::get_facets() const
{
    CALL_STACK_MSG();
    Int depth;
    PETSC_CHECK(DMPlexGetDepth(get_dm(), &depth));
    IS face_is;
    PETSC_CHECK(DMGetStratumIS(get_dm(), "dim", depth - 1, &face_is));
    if (!face_is)
        PETSC_CHECK(DMGetStratumIS(get_dm(), "depth", depth - 1, &face_is));
    return IndexSet(face_is);
}

Range
UnstructuredMesh::get_chart() const
{
    CALL_STACK_MSG();
    Int start, end;
    PETSC_CHECK(DMPlexGetChart(get_dm(), &start, &end));
    return Range(start, end);
}

void
UnstructuredMesh::set_chart(Int start, Int end)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexSetChart(get_dm(), start, end));
}

PolytopeType
UnstructuredMesh::get_cell_type(Int el) const
{
    CALL_STACK_MSG();
    DMPolytopeType polytope_type;
    PETSC_CHECK(DMPlexGetCellType(get_dm(), el, &polytope_type));
    return static_cast<PolytopeType>(polytope_type);
}

void
UnstructuredMesh::create_closure_index()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexCreateClosureIndex(get_dm(), nullptr));
}

void
UnstructuredMesh::create_closure_index(const Section & section)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexCreateClosureIndex(get_dm(), section));
}

std::vector<Int>
UnstructuredMesh::get_connectivity(Int point) const
{
    CALL_STACK_MSG();
    Int closure_size;
    Int * closure = nullptr;
    PETSC_CHECK(DMPlexGetTransitiveClosure(get_dm(), point, PETSC_TRUE, &closure_size, &closure));
    auto polytope_type = get_cell_type(point);
    Int n_elem_nodes = UnstructuredMesh::get_num_cell_nodes(polytope_type);
    std::vector<Int> elem_connect;
    elem_connect.resize(n_elem_nodes);
    for (Int k = 0; k < n_elem_nodes; ++k) {
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
    CALL_STACK_MSG();
    Int n_support;
    PETSC_CHECK(DMPlexGetSupportSize(get_dm(), point, &n_support));
    const Int * support;
    PETSC_CHECK(DMPlexGetSupport(get_dm(), point, &support));
    std::vector<Int> v;
    v.resize(n_support);
    for (Int i = 0; i < n_support; ++i)
        v[i] = support[i];
    return v;
}

Int
UnstructuredMesh::get_support_size(Int point) const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMPlexGetSupportSize(get_dm(), point, &n));
    return n;
}

std::vector<Int>
UnstructuredMesh::get_cone(Int point) const
{
    CALL_STACK_MSG();
    Int n = get_cone_size(point);
    const Int * cone;
    PETSC_CHECK(DMPlexGetCone(get_dm(), point, &cone));
    std::vector<Int> v;
    v.resize(n);
    for (Int i = 0; i < n; ++i)
        v[i] = cone[i];
    return v;
}

Int
UnstructuredMesh::get_cone_size(Int point) const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMPlexGetConeSize(get_dm(), point, &n));
    return n;
}

IndexSet
UnstructuredMesh::get_cone_recursive_vertices(IndexSet points) const
{
    CALL_STACK_MSG();
    IndexSet expanded_points;
    PETSC_CHECK(DMPlexGetConeRecursiveVertices(get_dm(), points, expanded_points));
    return expanded_points;
}

void
UnstructuredMesh::set_cone_size(Int point, Int size)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexSetConeSize(get_dm(), point, size));
}

void
UnstructuredMesh::set_cone(Int point, const std::vector<Int> & cone)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexSetCone(get_dm(), point, cone.data()));
}

Int
UnstructuredMesh::get_point_depth(Int point) const
{
    CALL_STACK_MSG();
    Int depth;
    PETSC_CHECK(DMPlexGetPointDepth(get_dm(), point, &depth));
    return depth;
}

void
UnstructuredMesh::set_partitioner(const Partitioner & part)
{
    PETSC_CHECK(DMPlexSetPartitioner(get_dm(), part));
}

void
UnstructuredMesh::distribute(Int overlap)
{
    CALL_STACK_MSG();

    DM dm_dist = nullptr;
    PETSC_CHECK(DMPlexDistribute(get_dm(), overlap, nullptr, &dm_dist));
    if (dm_dist)
        set_dm(dm_dist);
}

bool
UnstructuredMesh::is_distributed() const
{
    CALL_STACK_MSG();
    PetscBool flag;
    PETSC_CHECK(DMPlexIsDistributed(get_dm(), &flag));
    return flag == PETSC_TRUE;
}

bool
UnstructuredMesh::is_simplex() const
{
    CALL_STACK_MSG();
    PetscBool simplex;
    PETSC_CHECK(DMPlexIsSimplex(get_dm(), &simplex));
    return simplex == PETSC_TRUE;
}

void
UnstructuredMesh::create_face_set_labels(const std::map<Int, std::string> & names)
{
    CALL_STACK_MSG();
    Label fs_label = get_label("Face Sets");
    if (fs_label) {
        Int n_fs = fs_label.get_num_values();
        IndexSet fs_ids = fs_label.get_value_index_set();
        fs_ids.get_indices();
        for (Int i = 0; i < n_fs; ++i) {
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
    CALL_STACK_MSG();
    Label face_sets_label = get_label("Face Sets");
    IndexSet is = face_sets_label.get_stratum(id);
    if (!is.empty()) {
        create_label(name);
        Label label = get_label(name);
        label.set_stratum(id, is);
    }
    is.destroy();
}

void
UnstructuredMesh::set_face_set_name(Int id, const std::string & name)
{
    CALL_STACK_MSG();
    this->face_set_names[id] = name;
    this->face_set_ids[name] = id;
}

bool
UnstructuredMesh::has_face_set(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->face_set_ids.find(name);
    return it != this->face_set_ids.end();
}

Label
UnstructuredMesh::get_face_set_label(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->face_set_ids.find(name);
    if (it != this->face_set_ids.end())
        return get_label(name);
    else
        return Label();
}

const std::string &
UnstructuredMesh::get_face_set_name(Int id) const
{
    CALL_STACK_MSG();
    const auto & it = this->face_set_names.find(id);
    if (it != this->face_set_names.end())
        return it->second;
    else
        throw Exception("Face set ID '{}' does not exist.", id);
}

void
UnstructuredMesh::create_cell_set(Int id, const std::string & name)
{
    CALL_STACK_MSG();
    auto cell_sets_label = get_label("Cell Sets");
    auto cell_set = cell_sets_label.get_stratum(id);
    create_label(name);
    auto label = get_label(name);
    if (!cell_set.empty())
        label.set_stratum(id, cell_set);
    cell_set.destroy();
}

void
UnstructuredMesh::set_cell_set_name(Int id, const std::string & name)
{
    CALL_STACK_MSG();
    this->cell_set_names[id] = name;
    this->cell_set_ids[name] = id;
}

void
UnstructuredMesh::set_cell_type(Int cell, PolytopeType cell_type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexSetCellType(get_dm(), cell, (DMPolytopeType) cell_type));
}

const std::string &
UnstructuredMesh::get_cell_set_name(Int id) const
{
    CALL_STACK_MSG();
    const auto & it = this->cell_set_names.find(id);
    if (it != this->cell_set_names.end())
        return it->second;
    else
        throw Exception("Cell set ID '{}' does not exist.", id);
}

Int
UnstructuredMesh::get_cell_set_id(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->cell_set_ids.find(name);
    if (it != this->cell_set_ids.end())
        return it->second;
    else
        throw Exception("Cell set '{}' does not exist.", name);
}

Int
UnstructuredMesh::get_num_cell_sets() const
{
    CALL_STACK_MSG();
    Int n_cells_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Cell Sets", &n_cells_sets));
    return n_cells_sets;
}

const std::map<Int, std::string> &
UnstructuredMesh::get_cell_sets() const
{
    CALL_STACK_MSG();
    return this->cell_set_names;
}

Int
UnstructuredMesh::get_num_face_sets() const
{
    CALL_STACK_MSG();
    Int n_face_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Face Sets", &n_face_sets));
    return n_face_sets;
}

const std::map<Int, std::string> &
UnstructuredMesh::get_face_sets() const
{
    CALL_STACK_MSG();
    return this->face_set_names;
}

const std::string &
UnstructuredMesh::get_vertex_set_name(Int id) const
{
    CALL_STACK_MSG();
    const auto & it = this->vertex_set_names.find(id);
    if (it != this->vertex_set_names.end())
        return it->second;
    else
        throw Exception("Vertex set ID '{}' does not exist.", id);
}

Int
UnstructuredMesh::get_num_vertex_sets() const
{
    CALL_STACK_MSG();
    Int n_vertex_sets;
    PETSC_CHECK(DMGetLabelSize(get_dm(), "Vertex Sets", &n_vertex_sets));
    return n_vertex_sets;
}

const std::map<Int, std::string> &
UnstructuredMesh::get_vertex_sets() const
{
    CALL_STACK_MSG();
    return this->vertex_set_names;
}

bool
UnstructuredMesh::has_vertex_set(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->vertex_set_ids.find(name);
    return it != this->vertex_set_ids.end();
}

Label
UnstructuredMesh::get_vertex_set_label(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->vertex_set_ids.find(name);
    if (it != this->vertex_set_ids.end())
        return get_label(name);
    else
        return Label();
}

void
UnstructuredMesh::create_vertex_set_labels(const std::map<Int, std::string> & names)
{
    CALL_STACK_MSG();
    auto vs_label = get_label("Vertex Sets");
    if (vs_label) {
        auto n_vs = vs_label.get_num_values();
        auto vs_ids = vs_label.get_value_index_set();
        vs_ids.get_indices();
        for (Int i = 0; i < n_vs; ++i) {
            Int id = vs_ids[i];
            create_vertex_set(id, names.at(id));
        }
        vs_ids.restore_indices();
        vs_ids.destroy();
    }
}

void
UnstructuredMesh::create_vertex_set(Int id, const std::string & name)
{
    CALL_STACK_MSG();
    auto vertex_sets_label = get_label("Vertex Sets");
    auto is = vertex_sets_label.get_stratum(id);
    if (!is.empty()) {
        create_label(name);
        auto label = get_label(name);
        label.set_stratum(id, is);
    }
    is.destroy();
}

void
UnstructuredMesh::set_vertex_set_name(Int id, const std::string & name)
{
    CALL_STACK_MSG();
    this->vertex_set_names[id] = name;
    this->vertex_set_ids[name] = id;
}

void
UnstructuredMesh::construct_ghost_cells()
{
    CALL_STACK_MSG();
    DM gdm;
    PETSC_CHECK(DMPlexConstructGhostCells(get_dm(), nullptr, nullptr, &gdm));
    set_dm(gdm);
}

void
UnstructuredMesh::compute_cell_geometry(Int cell, Real * vol, Real centroid[], Real normal[]) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexComputeCellGeometryFVM(get_dm(), cell, vol, centroid, normal));
}

Real
UnstructuredMesh::compute_cell_volume(Int cell) const
{
    CALL_STACK_MSG();
    Real volume;
    PETSC_CHECK(DMPlexComputeCellGeometryFVM(get_dm(), cell, &volume, nullptr, nullptr));
    return volume;
}

int
UnstructuredMesh::get_num_cell_nodes(PolytopeType elem_type)
{
    CALL_STACK_MSG();
    switch (elem_type) {
    case PolytopeType::POINT:
        return 1;
    case PolytopeType::SEGMENT:
        return 2;
    case PolytopeType::TRIANGLE:
        return 3;
    case PolytopeType::QUADRILATERAL:
    case PolytopeType::TETRAHEDRON:
        return 4;
    case PolytopeType::HEXAHEDRON:
        return 8;
    default:
        error("Unsupported type '{}'.", conv::to_str(elem_type));
    }
}

const std::map<Int, std::vector<Int>> &
UnstructuredMesh::common_cells_by_vertex()
{
    CALL_STACK_MSG();
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

void
UnstructuredMesh::mark_boundary_faces(Int val, Label & label)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexMarkBoundaryFaces(get_dm(), val, label));
}

UnstructuredMesh *
UnstructuredMesh::build_from_cell_list(const mpi::Communicator & comm,
                                       Int dim,
                                       Int n_corners,
                                       const std::vector<Int> & cells,
                                       Int space_dim,
                                       const std::vector<Real> & vertices,
                                       bool interpolate)
{
    CALL_STACK_MSG();
    DM dm;
    PETSC_CHECK(DMPlexCreateFromCellListPetsc(comm,
                                              dim,
                                              cells.size() / n_corners,
                                              vertices.size() / space_dim,
                                              n_corners,
                                              interpolate ? PETSC_TRUE : PETSC_FALSE,
                                              cells.data(),
                                              space_dim,
                                              vertices.data(),
                                              &dm));
    return new UnstructuredMesh(dm);
}

StarForest
UnstructuredMesh::get_point_star_forest() const
{
    CALL_STACK_MSG();
    PetscSF sf;
    PETSC_CHECK(DMGetPointSF(get_dm(), &sf));
    return { sf };
}

void
UnstructuredMesh::set_point_star_forest(const StarForest & sf)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetPointSF(get_dm(), sf));
}

void
UnstructuredMesh::interpolate()
{
    CALL_STACK_MSG();
    DM idm;
    PETSC_CHECK(DMPlexInterpolate(get_dm(), &idm));
    set_dm(idm);
}

void
UnstructuredMesh::symmetrize()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexSymmetrize(get_dm()));
}

void
UnstructuredMesh::stratify()
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexStratify(get_dm()));
}

std::vector<Int>
UnstructuredMesh::get_full_join(const std::vector<Int> & points)
{
    Int n_covered_points;
    const Int * covered_points;
    PETSC_CHECK(DMPlexGetFullJoin(get_dm(),
                                  points.size(),
                                  points.data(),
                                  &n_covered_points,
                                  &covered_points));

    std::vector<Int> out_points(n_covered_points);
    for (Int i = 0; i < n_covered_points; ++i)
        out_points[i] = covered_points[i];

    PETSC_CHECK(DMPlexRestoreJoin(get_dm(),
                                  points.size(),
                                  points.data(),
                                  &n_covered_points,
                                  &covered_points));

    return out_points;
}

std::vector<Real>
UnstructuredMesh::get_vertex_coordinates(Int pt) const
{
    Real * data;
    auto dm_coord = get_coordinate_dm();
    auto coord_vec = get_coordinates_local();
    auto coord_array = coord_vec.get_array_read();
    PETSC_CHECK(DMPlexPointLocalRead(dm_coord, pt, coord_array, &data));
    coord_vec.restore_array_read(coord_array);
    auto dim = get_dimension();
    std::vector<Real> coord(dim);
    for (Int i = 0; i < dim; ++i)
        coord[i] = data[i];
    return coord;
}

Int
UnstructuredMesh::get_polytope_dim(PolytopeType type)
{
    CALL_STACK_MSG();
    return DMPolytopeTypeGetDim((DMPolytopeType) type);
}

void
UnstructuredMesh::invert_cell(PolytopeType type, std::vector<Int> & cone)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMPlexInvertCell((DMPolytopeType) type, cone.data()));
}

IndexSet
UnstructuredMesh::get_cell_numbering() const
{
    CALL_STACK_MSG();
    IS is;
    PETSC_CHECK(DMPlexGetCellNumbering(get_dm(), &is));
    return IndexSet(is);
}

void
UnstructuredMesh::create_my_cells_label()
{
    CALL_STACK_MSG();
    create_label("my_cells");
    auto label = get_label("my_cells");
    auto sf = get_point_star_forest();
    auto graph = sf.get_graph();
    if (graph) {
        for (auto & cell : get_cell_range()) {
            auto idx = graph.find_leaf(cell);
            if (idx >= 0)
                label.set_value(cell, 0);
            else
                label.set_value(cell, 1);
        }
    }
    else {
        for (auto & cell : get_cell_range())
            label.set_value(cell, 1);
    }
}

void
UnstructuredMesh::create_my_facets_label()
{
    CALL_STACK_MSG();
    auto sf = get_point_star_forest();
    auto graph = sf.get_graph();
    create_label("my_facets");
    auto label = get_label("my_facets");
    if (graph) {
        for (auto & facet : get_face_range()) {
            auto idx = graph.find_leaf(facet);
            if (idx >= 0)
                label.set_value(facet, 0);
            else
                label.set_value(facet, 1);
        }
    }
    else {
        for (auto & cell : get_face_range())
            label.set_value(cell, 1);
    }
}

bool
UnstructuredMesh::is_my_cell(Int cell) const
{
    CALL_STACK_MSG();
    auto my_cells = get_label("my_cells");
    assert(my_cells);
    return my_cells.get_value(cell) == 1;
}

} // namespace godzilla
