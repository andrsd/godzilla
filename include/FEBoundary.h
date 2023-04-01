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

namespace godzilla {

namespace fe {

class BoundaryInfoAbstract {};

template <ElementType ELEM_TYPE, Int DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class BoundaryInfo : public BoundaryInfoAbstract {
public:
    BoundaryInfo(const UnstructuredMesh * mesh,
                 const Array1D<DenseVector<Real, DIM>> * coords,
                 const Array1D<Real> * fe_volume,
                 const Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> * grad_phi,
                 const IndexSet & facets) :
        mesh(mesh),
        coords(coords),
        fe_volume(fe_volume),
        grad_phi(grad_phi),
        facets(facets)
    {
    }

    void
    create()
    {
        this->facets.get_indices();
        allocate();
        calc_face_normals();
    }

    void
    destroy()
    {
        free();
        this->facets.restore_indices();
        this->facets.destroy();
    }

protected:
    void
    allocate()
    {
        Int n = this->facets.get_local_size();
        this->length.create(n);
        this->normal.create(n);
    }

    void
    free()
    {
        this->length.destroy();
        this->normal.destroy();
    }

    Int
    get_local_face_index(DenseVector<Int, N_ELEM_NODES> & elem_conn,
                         DenseVector<Int, N_ELEM_NODES - 1> & face_conn)
    {
        // this only works for simplexes
        std::vector<Int> diff;
        std::set_difference(elem_conn.begin(),
                            elem_conn.end(),
                            face_conn.begin(),
                            face_conn.end(),
                            std::inserter(diff, diff.begin()));
        assert(diff.size() == 1);
        auto it = std::find(elem_conn.begin(), elem_conn.end(), diff[0]);
        return std::distance(elem_conn.begin(), it);
    }

    /// Compute face normals
    void
    calc_face_normals()
    {
        _F_;
        for (Int i = 0; i < this->facets.get_local_size(); i++) {
            auto face_conn = this->mesh->get_cell_connectivity(this->facets(i));

            auto support = this->mesh->get_support(this->facets(i));
            Int ie = support[0];

            auto volume = (*this->fe_volume)(ie);

            auto elem_conn = this->mesh->get_cell_connectivity(ie);

            int local_idx = get_local_face_index(elem_conn, face_conn);

            DenseVector<Int, 2> idx({ face_conn });
            auto face_coords = this->coords->get_values(idx);
            auto edge_length = fe::face_area<ELEM_TYPE>(face_coords);
            this->length(i) = edge_length;

            auto grad = (*this->grad_phi)(ie) (local_idx);
            auto normal = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
            this->normal(i) = normal;
        }
    }

    /// Mesh
    const UnstructuredMesh * mesh;
    /// Coordinates
    const Array1D<DenseVector<Real, DIM>> * coords;
    /// Element volume
    const Array1D<Real> * fe_volume;
    /// Gradients of shape functions
    const Array1D<DenseVector<DenseVector<Real, DIM>, N_ELEM_NODES>> * grad_phi;
    /// IndexSet with boundary facets
    IndexSet facets;
    /// Boundary face length (?)
    Array1D<Real> length;
    /// Boundary face unit outward normal
    Array1D<DenseVector<Real, DIM>> normal;
};

} // namespace fe

} // namespace godzilla
