// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/IndexSet.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/DenseVector.h"
#include "godzilla/DenseMatrix.h"
#include "godzilla/Array1D.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/FEShapeFns.h"
#include "godzilla/Utils.h"
#include <set>
#include <vector>
#include "petscdm.h"

namespace godzilla {

namespace fe {

class BoundaryInfoAbstract {
public:
    virtual ~BoundaryInfoAbstract() = default;

    /// Create boundary info
    virtual void create() = 0;

    /// Destroy boundary info
    virtual void destroy() = 0;
};

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class BoundaryInfo : public BoundaryInfoAbstract {
public:
    BoundaryInfo(UnstructuredMesh * mesh,
                 const Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> * grad_phi,
                 const IndexSet & facets) :
        mesh(mesh),
        grad_phi(grad_phi),
        facets(facets),
        area(length)
    {
        CALL_STACK_MSG();
        this->facets.sort();
    }

    BoundaryInfo(UnstructuredMesh * mesh, const IndexSet & facets) :
        mesh(mesh),
        grad_phi(nullptr),
        facets(facets),
        area(length)
    {
        CALL_STACK_MSG();
        this->facets.sort();
    }

    UnstructuredMesh *
    get_mesh() const
    {
        return this->mesh;
    }

protected:
    void
    create_vertex_index_set()
    {
        CALL_STACK_MSG();
        if (!this->vertices) {
            this->vertices = this->mesh->get_cone_recursive_vertices(this->facets);
            this->vertices.sort_remove_dups();
            this->vertices.get_indices();
        }
    }

    void
    compute_face_normals()
    {
        CALL_STACK_MSG();
        if (this->facets) {
            this->facets.get_indices();
            Int n = this->facets.get_local_size();
            this->length.create(n);
            this->normal.create(n);

            calc_face_length();
            calc_face_normals();
        }
    }

    void
    compute_nodal_normals()
    {
        CALL_STACK_MSG();
        if (this->vertices) {
            Int n = this->vertices.get_local_size();
            this->nodal_normal.create(n);
            calc_nodal_normals();
        }
    }

    void
    free()
    {
        CALL_STACK_MSG();
        if (this->facets) {
            this->facets.restore_indices();
            this->facets.destroy();
            this->length.destroy();
            this->normal.destroy();
        }
        if (this->vertices) {
            this->vertices.restore_indices();
            this->vertices.destroy();
            this->nodal_normal.destroy();
        }
    }

private:
    inline DenseMatrix<Real, DIM, N_ELEM_NODES>
    calc_grad_shape(Int cell, Real volume) const
    {
        if (this->grad_phi)
            return (*this->grad_phi)(cell);
        else {
            auto dm = this->mesh->get_coordinate_dm();
            auto vec = this->mesh->get_coordinates_local();
            auto section = this->mesh->get_coordinate_section();
            DenseMatrix<Real, N_ELEM_NODES, DIM> elem_coord;
            Int sz = DIM * N_ELEM_NODES;
            Real * data = elem_coord.data();
            PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, cell, &sz, &data));
            return fe::grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
        }
    }

    /// Compute face normals
    void
    calc_face_normals()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets.get_local_size(); ++i) {
            auto face_conn = this->mesh->get_connectivity(this->facets(i));
            auto support = this->mesh->get_support(this->facets(i));
            Int ie = support[0];
            Real volume = this->mesh->compute_cell_volume(ie);
            auto elem_conn = this->mesh->get_connectivity(ie);
            Int local_idx = get_local_face_index(elem_conn, face_conn);
            auto edge_length = this->length(i);
            auto grad = DenseVector<Real, DIM>(calc_grad_shape(ie, volume).column(local_idx));
            this->normal(i) = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
        }
    }

    void
    calc_face_length()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets.get_local_size(); ++i)
            this->length(i) = this->mesh->compute_cell_volume(this->facets(i));
    }

    /// Compute nodal normals
    void
    calc_nodal_normals()
    {
        CALL_STACK_MSG();
        auto comm_cells = this->mesh->common_cells_by_vertex();
        for (Int i = 0; i < this->vertices.get_local_size(); ++i) {
            Int vertex = this->vertices(i);
            DenseVector<Real, DIM> sum;
            sum.zero();
            for (auto & cell : comm_cells[vertex]) {
                Real volume = this->mesh->compute_cell_volume(cell);
                auto connect = this->mesh->get_connectivity(cell);
                auto lnne = utils::index_of(connect, vertex);
                auto inc =
                    DenseVector<Real, DIM>(volume * calc_grad_shape(cell, volume).column(lnne));
                sum += inc;
            }
            auto mag = sum.magnitude();
            this->nodal_normal(i) = (1. / mag) * sum;
        }
        correct_nodal_normals();
    }

    void
    correct_nodal_normals()
    {
        CALL_STACK_MSG();
    }

    /// Mesh
    UnstructuredMesh * mesh;
    /// Gradients of shape functions
    const Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> * grad_phi;

public:
    /// IndexSet with boundary facets
    IndexSet facets;
    /// Boundary face length
    Array1D<Real> length;
    /// Boundary face area
    Array1D<Real> & area;
    /// Boundary face unit outward normal
    Array1D<DenseVector<Real, DIM>> normal;
    /// IndexSet with boundary vertices
    IndexSet vertices;
    /// Boundary node unit outward normal
    Array1D<DenseVector<Real, DIM>> nodal_normal;
};

template <>
inline void
BoundaryInfo<TRI3, 2, 3>::correct_nodal_normals()
{
    CALL_STACK_MSG();
    std::map<Int, Int> idx_of;
    for (Int i = 0; i < this->facets.get_local_size(); ++i) {
        idx_of.insert(std::pair<Int, Int>(this->facets[i], i));
    }

    for (Int i = 0; i < this->vertices.get_local_size(); ++i) {
        Int vertex = this->vertices(i);
        auto support = this->mesh->get_support(vertex);

        std::vector<Int> common_edges;
        std::sort(support.begin(), support.end());
        std::set_intersection(support.begin(),
                              support.end(),
                              this->facets.begin(),
                              this->facets.end(),
                              std::back_inserter(common_edges));
        if (common_edges.size() == 1) {
            Int face_normal_idx = idx_of[common_edges[0]];
            this->nodal_normal(i) = this->normal(face_normal_idx);
        }
    }
}

template <>
inline void
BoundaryInfo<TET4, 3, 4>::correct_nodal_normals()
{
    CALL_STACK_MSG();
    // Mapping from vertex number to local vertex number (0..num_of_vertices)
    std::map<Int, Int> idx_of_vertex;
    for (auto & i : make_range(this->vertices.get_local_size()))
        idx_of_vertex.insert(std::make_pair(this->vertices[i], i));
    // Mapping from facet number to local facet number (0..num_of_facets)
    std::map<Int, Int> idx_of_face;
    for (auto & i : make_range(this->facets.get_local_size()))
        idx_of_face.insert(std::make_pair(this->facets[i], i));

    // Number of faces connected to an edge
    // If edge has only 1 connected face it is an boundary edge
    std::map<Int, Int> edge_bnd;
    for (auto & i : make_range(this->facets.get_local_size())) {
        auto facet = this->facets[i];
        auto face_edges = this->mesh->get_cone(facet);
        for (auto edge : face_edges)
            edge_bnd[edge]++;
    }
    // Number of boundary edges per vertex. Vertices with 2 boundary edges are boundary vertices
    // and we have to correct normals at those locations
    std::map<Int, Int> vertex_bnd_edges;
    for (auto & i : make_range(this->facets.get_local_size())) {
        auto facet = this->facets[i];
        auto face_edges = this->mesh->get_cone(facet);
        for (auto edge : face_edges) {
            if (edge_bnd[edge] == 1) {
                auto vtxs = this->mesh->get_cone(edge);
                for (auto & v : vtxs)
                    vertex_bnd_edges[v]++;
            }
        }
    }
    // Find facets connected to a boundary vertices
    std::map<Int, std::set<Int>> vertex_facets;
    for (auto & i : make_range(this->facets.get_local_size())) {
        auto facet = this->facets[i];
        std::set<Int> face_vtxs;
        auto face_edges = this->mesh->get_cone(facet);
        for (auto edge : face_edges) {
            auto vtxs = this->mesh->get_cone(edge);
            for (auto & v : vtxs)
                face_vtxs.insert(v);
        }
        for (auto & v : face_vtxs)
            if (vertex_bnd_edges[v] == 2)
                vertex_facets[v].insert(facet);
    }
    // A normal at a boundary vertex will be computed as an average of face normals from connected
    // facets
    for (auto & [vtx, connected_facets] : vertex_facets) {
        DenseVector<Real, 3> normal({ 0, 0, 0 });
        for (auto & face : connected_facets) {
            Int face_normal_idx = idx_of_face[face];
            normal += this->normal(face_normal_idx);
        }
        normal.scale(1. / connected_facets.size());
        normal.normalize();
        auto vtx_idx = idx_of_vertex[vtx];
        this->nodal_normal(vtx_idx) = normal;
    }
}

///

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class EssentialBoundaryInfo : public BoundaryInfoAbstract {
public:
    EssentialBoundaryInfo(UnstructuredMesh * mesh, const IndexSet & vertices) :
        mesh(mesh),
        vertices(vertices)
    {
        CALL_STACK_MSG();
    }

    UnstructuredMesh *
    get_mesh() const
    {
        return this->mesh;
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        this->vertices.get_indices();
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        this->vertices.restore_indices();
    }

private:
    /// Mesh
    UnstructuredMesh * mesh;

public:
    /// IndexSet with boundary vertices
    IndexSet vertices;
};

} // namespace fe

} // namespace godzilla
