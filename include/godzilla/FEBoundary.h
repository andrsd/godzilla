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
#include "godzilla/FEShapeFns.h"
#include "godzilla/Assert.h"
#include "petscdm.h"

namespace godzilla {

namespace fe {

/// Function that can be called from boundary vertex/facet loop
template <typename F>
concept BoundaryFunction = std::is_invocable_r_v<void, F, Int, Int>;

class AbstractBoundaryInfo {
public:
    virtual ~AbstractBoundaryInfo() = default;

    /// Create boundary info
    virtual void create() = 0;

    /// Destroy boundary info
    virtual void destroy() = 0;
};

/// Essential boundary info

template <ElementType ELEM_TYPE, Dimension DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class EssentialBoundaryInfo : public AbstractBoundaryInfo {
public:
    EssentialBoundaryInfo(Ref<UnstructuredMesh> mesh, IndexSet vertices) :
        mesh_(mesh),
        vertices_(vertices)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
    }

    Ref<UnstructuredMesh>
    get_mesh() const
    {
        return this->mesh_;
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        if (this->vertices_)
            this->vtx_idxs_ = this->vertices_.borrow_indices();
    }

    void
    destroy() override
    {
    }

    /// Get vertex index
    ///
    /// @param idx Boundary vertex index (local)
    /// @return Global vertex index
    Int
    vertex(Int ibn) const
    {
        CALL_STACK_MSG();
        return this->vtx_idxs_[ibn];
    }

    /// Get the number of boundary vertices
    ///
    /// @return Number of boundary vertices
    Int
    num_vertices() const
    {
        CALL_STACK_MSG();
        if (!this->vertices_.is_null())
            return this->vertices_.get_local_size();
        else
            return 0;
    }

    /// Iterate over all boundary vertices
    template <BoundaryFunction Func>
    void
    for_each_vertex(Func fn)
    {
        for (auto & ibn : make_range(this->num_vertices())) {
            auto vertex_idx = this->vtx_idxs_[ibn];
            fn(ibn, vertex_idx);
        }
    }

private:
    /// Mesh
    Ref<UnstructuredMesh> mesh_;
    /// IndexSet with boundary vertices
    IndexSet vertices_;
    /// Vertex indices
    IndexSetBorrowedIndices vtx_idxs_;
};

/// Natural boundary information

template <ElementType ELEM_TYPE, Dimension DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class NaturalBoundaryInfo : public AbstractBoundaryInfo {
public:
    NaturalBoundaryInfo(Ref<UnstructuredMesh> mesh,
                        Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_phi,
                        IndexSet facets) :
        mesh_(mesh),
        grad_phi_(grad_phi),
        facets_(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        if (!this->facets_.is_null())
            this->facets_.sort();
    }

    NaturalBoundaryInfo(Ref<UnstructuredMesh> mesh, IndexSet facets) : mesh_(mesh), facets_(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        if (!this->facets_.is_null())
            this->facets_.sort();
    }

    void
    create() override
    {
        CALL_STACK_MSG();
        this->compute_face_normals();
    }

    void
    destroy() override
    {
        CALL_STACK_MSG();
        this->free();
    }

    Ref<UnstructuredMesh>
    get_mesh() const
    {
        return this->mesh_;
    }

    /// Get number of boundary facets
    ///
    /// @return Number of boundary facets
    Int
    num_facets() const
    {
        CALL_STACK_MSG();
        if (!this->facets_.is_null())
            return this->facets_.get_local_size();
        else
            return 0;
    }

    /// Get facet index for a given local boundary facet index
    ///
    /// @param ibf Local boundary facet index
    /// @return Global facet index
    Int
    facet(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->facet_idxs_[ibf];
    }

    /// Get face normal for a given local boundary facet index
    ///
    /// @param ibf Local boundary facet index
    /// @return Face normal
    const DenseVector<Real, DIM> &
    normal(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->normals_[ibf];
    }

    /// Get length/area of a boundary facet
    ///
    /// @param ibf Local boundary facet index
    /// @return Length/area of the boundary facet
    Real
    facet_length(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->lengths_[ibf];
    }

    /// Get area of a boundary facet
    ///
    /// @param ibf Local boundary facet index
    /// @return Area of the boundary facet
    Real
    facet_area(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->lengths_[ibf];
    }

    /// Iterate over all boundary facets
    template <BoundaryFunction Func>
    void
    for_each_facet(Func fn)
    {
        for (auto & ibf : make_range(this->num_facets())) {
            auto facet = this->facet_idxs_[ibf];
            fn(ibf, facet);
        }
    }

protected:
    void
    compute_face_normals()
    {
        CALL_STACK_MSG();
        if (this->facets_) {
            this->facet_idxs_ = this->facets_.borrow_indices();
            Int n = this->facets_.get_local_size();
            this->lengths_ = Array1D<Real>(mesh_->get_comm(), n);
            this->normals_ = Array1D<DenseVector<Real, DIM>>(mesh_->get_comm(), n);

            calc_facet_lengths();
            calc_facet_normals();
        }
    }

    void
    free()
    {
        CALL_STACK_MSG();
    }

private:
    inline DenseMatrix<Real, DIM, N_ELEM_NODES>
    calc_grad_shape(Int cell, Real volume) const
    {
        if (this->grad_phi_)
            return this->grad_phi_[cell];
        else {
            auto dm = this->mesh_->get_coordinate_dm();
            auto vec = this->mesh_->get_coordinates_local();
            auto section = this->mesh_->get_coordinate_section();
            DenseMatrix<Real, N_ELEM_NODES, DIM> elem_coord;
            Int sz = DIM * N_ELEM_NODES;
            Real * data = elem_coord.data();
            PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, cell, &sz, &data));
            return fe::grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
        }
    }

    /// Compute facet normals
    void
    calc_facet_normals()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets_.get_local_size(); ++i) {
            auto facet = this->facet_idxs_[i];
            auto face_conn = this->mesh_->get_connectivity(facet);
            auto support = this->mesh_->get_support(facet);
            Int ie = support[0];
            auto cone = this->mesh_->get_cone(ie);
            auto local_face_idx = utils::index_of(cone, facet);
            auto grad_fn_idx = fe::get_grad_fn_index<ELEM_TYPE, DIM, N_ELEM_NODES>(local_face_idx);
            auto volume = this->mesh_->compute_cell_volume(ie);
            auto edge_length = this->lengths_[i];
            DenseVector<Real, DIM> grad(calc_grad_shape(ie, volume).column(grad_fn_idx));
            this->normals_[i] = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
        }
    }

    /// Compute facet lengths/areas
    void
    calc_facet_lengths()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets_.get_local_size(); ++i)
            this->lengths_[i] = this->mesh_->compute_cell_volume(this->facet_idxs_[i]);
    }

private:
    /// Mesh
    Ref<UnstructuredMesh> mesh_;
    /// Gradients of shape functions
    Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_phi_;
    /// IndexSet with boundary facets
    IndexSet facets_;
    /// Facet indices
    IndexSetBorrowedIndices facet_idxs_;
    /// Boundary facet length
    Array1D<Real> lengths_;
    /// Boundary facet unit outward normal
    Array1D<DenseVector<Real, DIM>> normals_;
};

} // namespace fe

} // namespace godzilla
