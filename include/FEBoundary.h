#pragma once

#include "CallStack.h"
#include "Types.h"
#include "IndexSet.h"
#include "UnstructuredMesh.h"
#include "DenseVector.h"
#include "DenseMatrix.h"
#include "Array1D.h"
#include "Array2D.h"
#include "FEGeometry.h"
#include "FEVolumes.h"
#include <set>
#include <vector>

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
    BoundaryInfo(const UnstructuredMesh * mesh,
                 const Array1D<DenseVector<Real, DIM>> * coords,
                 const Array1D<DenseVector<Int, N_ELEM_NODES>> * connect,
                 const Array1D<std::vector<Int>> * nelcom,
                 const Array1D<Real> * fe_volume,
                 const Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> * grad_phi,
                 const IndexSet & facets) :
        mesh(mesh),
        coords(coords),
        connect(connect),
        nelcom(nelcom),
        fe_volume(fe_volume),
        grad_phi(grad_phi),
        facets(facets)
    {
        _F_;
    }

protected:
    void
    compute_face_normals()
    {
        _F_;
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
        _F_;
        for (Int i = 0; i < this->facets.get_local_size(); i++) {
            auto face_conn = this->mesh->get_connectivity(this->facets(i));
            auto support = this->mesh->get_support(this->facets(i));
            Int ie = support[0];
            auto volume = (*this->fe_volume)(ie);
            auto elem_conn = this->mesh->get_connectivity(ie);
            Int local_idx = get_local_face_index(elem_conn, face_conn);
            auto edge_length = this->length(i);
            auto grad = (*this->grad_phi)(ie) (local_idx);
            auto normal = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
            this->normal(i) = normal;
        }
    }

    void
    calc_face_length()
    {
        auto n_cells = this->mesh->get_num_all_cells();
        for (Int i = 0; i < this->facets.get_local_size(); i++) {
            auto face_conn = this->mesh->get_connectivity(this->facets(i));
            // works for simplexes
            DenseVector<Int, N_ELEM_NODES - 1> idx;
            for (Int j = 0; j < N_ELEM_NODES - 1; j++)
                idx(j) = face_conn[j] - n_cells;
            auto coords = this->coords->get_values(idx);
            auto edge_length = fe::face_area<ELEM_TYPE>(coords);
            this->length(i) = edge_length;
        }
    }

    /// Compute nodal normals
    void
    calc_nodal_normals()
    {
        _F_;
        auto vertex_range = this->mesh->get_vertex_range();
        for (Int i = 0; i < this->vertices.get_local_size(); i++) {
            Int vertex = this->vertices(i);
            Int node = vertex - vertex_range.get_first();
            DenseVector<Real, DIM> sum;
            sum.zero();
            for (Int iec = 0; iec < (*this->nelcom)(node).size(); iec++) {
                auto ie = (*this->nelcom)(node)[iec];
                auto idx = (*this->connect)(ie);
                auto lnne = node_index(idx, node);
                auto vol = (*this->fe_volume)(ie);
                auto inc = vol * (*this->grad_phi)(ie) (lnne);
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
        _F_;
    }

    Int
    node_index(const DenseVector<Int, N_ELEM_NODES> & idx, Int node_id)
    {
        for (Int i = 0; i < N_ELEM_NODES; i++)
            if (idx(i) == node_id)
                return i;
        error("Did not find {} in indices", node_id);
    }

    /// Mesh
    const UnstructuredMesh * mesh;
    /// Coordinates
    const Array1D<DenseVector<Real, DIM>> * coords;
    /// Connectivity array
    const Array1D<DenseVector<Int, N_ELEM_NODES>> * connect;
    /// Indices of elements common to a node
    const Array1D<std::vector<Int>> * nelcom;
    /// Element volume
    const Array1D<Real> * fe_volume;
    /// Gradients of shape functions
    const Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> * grad_phi;

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
    _F_;
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
