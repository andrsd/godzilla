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

/// Essential boundary info

template <ElementType ELEM_TYPE, Dimension DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class EssentialBoundaryInfo {
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

    /// Get vertex index set (all vertices this boundary info operates on)
    IndexSet
    vertices()
    {
        CALL_STACK_MSG();
        return this->vertices_;
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
        auto vtx_idxs = this->vertices_.borrow_indices();
        for (auto & ibn : make_range(this->num_vertices())) {
            auto vertex_idx = vtx_idxs[ibn];
            fn(ibn, vertex_idx);
        }
    }

private:
    /// Mesh
    Ref<UnstructuredMesh> mesh_;
    /// IndexSet with boundary vertices
    IndexSet vertices_;
};

/// Natural boundary information

template <ElementType ELEM_TYPE, Dimension DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class NaturalBoundaryInfo {
public:
    NaturalBoundaryInfo(Ref<UnstructuredMesh> mesh,
                        Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_phi,
                        IndexSet facets) :
        mesh_(mesh),
        facets_(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        if (!this->facets_.is_null()) {
            this->facets_.sort();

            Int n = this->facets_.get_local_size();
            this->lengths_ = Array1D<Real>(this->mesh_->get_comm(), n);
            this->normals_ = Array1D<DenseVector<Real, DIM>>(this->mesh_->get_comm(), n);

            calc_facet_lengths();
            calc_facet_normals(&grad_phi);
        }
    }

    NaturalBoundaryInfo(Ref<UnstructuredMesh> mesh, IndexSet facets) : mesh_(mesh), facets_(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        if (!this->facets_.is_null()) {
            this->facets_.sort();

            Int n = this->facets_.get_local_size();
            this->lengths_ = Array1D<Real>(this->mesh_->get_comm(), n);
            this->normals_ = Array1D<DenseVector<Real, DIM>>(this->mesh_->get_comm(), n);

            calc_facet_lengths();
            calc_facet_normals();
        }
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

    IndexSet
    facets()
    {
        CALL_STACK_MSG();
        return this->facets_;
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
        auto facet_idxs = this->facets_.borrow_indices();
        for (auto & ibf : make_range(this->num_facets())) {
            auto facet = facet_idxs[ibf];
            fn(ibf, facet);
        }
    }

private:
    inline DenseMatrix<Real, DIM, N_ELEM_NODES>
    calc_grad_shape(Int cell, Real volume) const
    {
        auto dm = this->mesh_->get_coordinate_dm();
        auto vec = this->mesh_->get_coordinates_local();
        auto section = this->mesh_->get_coordinate_section();
        DenseMatrix<Real, N_ELEM_NODES, DIM> elem_coord;
        Int sz = DIM * N_ELEM_NODES;
        Real * data = elem_coord.data();
        PETSC_CHECK(DMPlexVecGetClosure(dm, section, vec, cell, &sz, &data));
        return fe::grad_shape<ELEM_TYPE, DIM>(elem_coord, volume);
    }

    /// Compute facet normals
    void
    calc_facet_normals(const Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> * grad_phi = nullptr)
    {
        CALL_STACK_MSG();
        auto facet_idxs = this->facets_.borrow_indices();
        for (Int i = 0; i < this->facets_.get_local_size(); ++i) {
            auto facet = facet_idxs[i];
            auto face_conn = this->mesh_->get_connectivity(facet);
            auto support = this->mesh_->get_support(facet);
            Int cell = support[0];
            auto cone = this->mesh_->get_cone(cell);
            auto local_face_idx = utils::index_of(cone, facet);
            auto grad_fn_idx = fe::get_grad_fn_index<ELEM_TYPE, DIM, N_ELEM_NODES>(local_face_idx);
            auto volume = this->mesh_->compute_cell_volume(cell);
            auto edge_length = this->lengths_[i];
            DenseVector<Real, DIM> grad;
            if (grad_phi == nullptr) {
                grad = calc_grad_shape(cell, volume).column(grad_fn_idx);
            }
            else {
                grad = (*grad_phi)[cell].column(grad_fn_idx);
            }
            this->normals_[i] = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
        }
    }

    /// Compute facet lengths/areas
    void
    calc_facet_lengths()
    {
        CALL_STACK_MSG();
        auto facet_idxs = this->facets_.borrow_indices();
        for (Int i = 0; i < this->facets_.get_local_size(); ++i)
            this->lengths_[i] = this->mesh_->compute_cell_volume(facet_idxs[i]);
    }

private:
    /// Mesh
    Ref<UnstructuredMesh> mesh_;
    /// IndexSet with boundary facets
    IndexSet facets_;
    /// Boundary facet length
    Array1D<Real> lengths_;
    /// Boundary facet unit outward normal
    Array1D<DenseVector<Real, DIM>> normals_;
};

} // namespace fe

} // namespace godzilla
