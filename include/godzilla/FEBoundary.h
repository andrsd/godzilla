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
#include "godzilla/Array2D.h"
#include "godzilla/FEGeometry.h"
#include "godzilla/FEVolumes.h"
#include "godzilla/Utils.h"
#include <set>
#include <vector>
#include "petscdm.h"

namespace godzilla {

namespace fe {

class BoundaryInfoAbstract {
public:
    /// Create boundary info
    virtual void create() = 0;

    /// Destroy boundary info
    virtual void destroy() = 0;
};

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class BoundaryInfo : public BoundaryInfoAbstract {
public:
    BoundaryInfo(UnstructuredMesh * mesh,
                 const Array1D<DenseMatrix<Real, N_ELEM_NODES, DIM>> * grad_phi,
                 const IndexSet & facets) :
        mesh(mesh),
        grad_phi(grad_phi),
        facets(facets)
    {
        CALL_STACK_MSG();
    }

    UnstructuredMesh *
    get_mesh() const
    {
        return this->mesh;
    }

protected:
    void
    compute_face_normals()
    {
        CALL_STACK_MSG();
        if (!this->facets.empty()) {
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
        this->vertices = this->mesh->get_cone_recursive_vertices(this->facets);
        this->vertices.sort_remove_dups();
        if (!this->vertices.empty()) {
            this->vertices.get_indices();
            Int n = this->vertices.get_local_size();
            this->nodal_normal.create(n);
            calc_nodal_normals();
        }
    }

    void
    free()
    {
        if (!this->facets.empty()) {
            this->facets.restore_indices();
            this->facets.destroy();
            this->length.destroy();
            this->normal.destroy();
        }
        if (!this->vertices.empty()) {
            this->vertices.restore_indices();
            this->vertices.destroy();
            this->nodal_normal.destroy();
        }
    }

private:
    /// Compute face normals
    void
    calc_face_normals()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets.get_local_size(); i++) {
            auto face_conn = this->mesh->get_connectivity(this->facets(i));
            auto support = this->mesh->get_support(this->facets(i));
            Int ie = support[0];
            Real volume;
            this->mesh->compute_cell_geometry(ie, &volume, nullptr, nullptr);
            auto elem_conn = this->mesh->get_connectivity(ie);
            Int local_idx = get_local_face_index(elem_conn, face_conn);
            auto edge_length = this->length(i);
            auto grad = (*this->grad_phi)(ie).row(local_idx);
            this->normal(i) = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
        }
    }

    void
    calc_face_length()
    {
        for (Int i = 0; i < this->facets.get_local_size(); i++) {
            Real A;
            this->mesh->compute_cell_geometry(this->facets(i), &A, nullptr, nullptr);
            this->length(i) = A;
        }
    }

    /// Compute nodal normals
    void
    calc_nodal_normals()
    {
        CALL_STACK_MSG();
        auto comm_cells = this->mesh->common_cells_by_vertex();
        for (Int i = 0; i < this->vertices.get_local_size(); i++) {
            Int vertex = this->vertices(i);
            DenseVector<Real, DIM> sum;
            sum.zero();
            for (auto & cell : comm_cells[vertex]) {
                Real vol;
                this->mesh->compute_cell_geometry(cell, &vol, nullptr, nullptr);
                auto connect = this->mesh->get_connectivity(cell);
                auto lnne = utils::index_of(connect, vertex);
                auto inc = vol * (*this->grad_phi)(cell).row(lnne);
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
    const Array1D<DenseMatrix<Real, N_ELEM_NODES, DIM>> * grad_phi;

public:
    /// IndexSet with boundary facets
    IndexSet facets;
    /// Boundary face length
    Array1D<Real> length;
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
    for (Int i = 0; i < this->facets.get_local_size(); i++)
        idx_of.insert(std::pair<Int, Int>(this->facets[i], i));

    for (Int i = 0; i < this->vertices.get_local_size(); i++) {
        Int vertex = this->vertices(i);
        auto support = this->mesh->get_support(vertex);
        IndexSet support_is = IndexSet::create_general(this->mesh->get_comm(), support);
        auto common_edges = IndexSet::intersect(this->facets, support_is);
        if (common_edges.get_size() == 1) {
            common_edges.get_indices();
            Int face_normal_idx = idx_of[common_edges[0]];
            this->nodal_normal(i) = this->normal(face_normal_idx);
            common_edges.restore_indices();
        }
    }
}

} // namespace fe

} // namespace godzilla
