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
    EssentialBoundaryInfo(UnstructuredMesh * mesh, IndexSet vertices) :
        mesh(mesh),
        vertices(vertices)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
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

    /// Get vertex index
    ///
    /// @param idx Boundary vertex index (local)
    /// @return Global vertex index
    Int
    vertex(Int ibn) const
    {
        CALL_STACK_MSG();
        return this->vertices(ibn);
    }

    /// Get the number of boundary vertices
    ///
    /// @return Number of boundary vertices
    Int
    num_vertices() const
    {
        CALL_STACK_MSG();
        return this->vertices.get_local_size();
    }

    /// Iterate over all boundary vertices
    template <BoundaryFunction Func>
    void
    for_each_vertex(Func fn)
    {
        for (auto & ibn : make_range(this->num_vertices())) {
            auto vertex_idx = this->vertices(ibn);
            fn(ibn, vertex_idx);
        }
    }

private:
    /// Mesh
    UnstructuredMesh * mesh;
    /// IndexSet with boundary vertices
    IndexSet vertices;
};

/// Natural boundary information

template <ElementType ELEM_TYPE, Dimension DIM, Int N_ELEM_NODES = get_num_element_nodes(ELEM_TYPE)>
class NaturalBoundaryInfo : public AbstractBoundaryInfo {
public:
    NaturalBoundaryInfo(UnstructuredMesh * mesh,
                        Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_phi,
                        IndexSet facets) :
        mesh(mesh),
        grad_phi(grad_phi),
        facets(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        this->facets.sort();
    }

    NaturalBoundaryInfo(UnstructuredMesh * mesh, IndexSet facets) : mesh(mesh), facets(facets)
    {
        CALL_STACK_MSG();
        expect_true(mesh->get_dimension() == DIM, "Mesh dimension mismatch");
        this->facets.sort();
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

    UnstructuredMesh *
    get_mesh() const
    {
        return this->mesh;
    }

    /// Get number of boundary facets
    ///
    /// @return Number of boundary facets
    Int
    num_facets() const
    {
        CALL_STACK_MSG();
        return this->facets.get_local_size();
    }

    /// Get facet index for a given local boundary facet index
    ///
    /// @param ibf Local boundary facet index
    /// @return Global facet index
    Int
    facet(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->facets(ibf);
    }

    /// Get face normal for a given local boundary facet index
    ///
    /// @param ibf Local boundary facet index
    /// @return Face normal
    const DenseVector<Real, DIM> &
    normal(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->normals[ibf];
    }

    /// Get length/area of a boundary facet
    ///
    /// @param ibf Local boundary facet index
    /// @return Length/area of the boundary facet
    Real
    facet_length(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->lengths[ibf];
    }

    /// Get area of a boundary facet
    ///
    /// @param ibf Local boundary facet index
    /// @return Area of the boundary facet
    Real
    facet_area(Int ibf) const
    {
        CALL_STACK_MSG();
        return this->lengths[ibf];
    }

    /// Iterate over all boundary facets
    template <BoundaryFunction Func>
    void
    for_each_facet(Func fn)
    {
        for (auto & ibf : make_range(this->num_facets())) {
            auto facet = this->facets(ibf);
            fn(ibf, facet);
        }
    }

protected:
    void
    compute_face_normals()
    {
        CALL_STACK_MSG();
        if (this->facets) {
            this->facets.get_indices();
            Int n = this->facets.get_local_size();
            this->lengths = Array1D<Real>(n);
            this->normals = Array1D<DenseVector<Real, DIM>>(n);

            calc_facet_lengths();
            calc_facet_normals();
        }
    }

    void
    free()
    {
        CALL_STACK_MSG();
        if (this->facets) {
            this->facets.restore_indices();
            this->facets.destroy();
        }
    }

private:
    inline DenseMatrix<Real, DIM, N_ELEM_NODES>
    calc_grad_shape(Int cell, Real volume) const
    {
        if (this->grad_phi)
            return this->grad_phi[cell];
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

    /// Compute facet normals
    void
    calc_facet_normals()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets.get_local_size(); ++i) {
            auto facet = this->facets(i);
            auto face_conn = this->mesh->get_connectivity(facet);
            auto support = this->mesh->get_support(facet);
            Int ie = support[0];
            auto cone = this->mesh->get_cone(ie);
            auto local_face_idx = utils::index_of(cone, facet);
            auto grad_fn_idx = fe::get_grad_fn_index<ELEM_TYPE, DIM, N_ELEM_NODES>(local_face_idx);
            auto volume = this->mesh->compute_cell_volume(ie);
            auto edge_length = this->lengths[i];
            DenseVector<Real, DIM> grad(calc_grad_shape(ie, volume).column(grad_fn_idx));
            this->normals[i] = fe::normal<ELEM_TYPE>(volume, edge_length, grad);
        }
    }

    /// Compute facet lengths/areas
    void
    calc_facet_lengths()
    {
        CALL_STACK_MSG();
        for (Int i = 0; i < this->facets.get_local_size(); ++i)
            this->lengths[i] = this->mesh->compute_cell_volume(this->facets(i));
    }

private:
    /// Mesh
    UnstructuredMesh * mesh;
    /// Gradients of shape functions
    Array1D<DenseMatrix<Real, DIM, N_ELEM_NODES>> grad_phi;
    /// IndexSet with boundary facets
    IndexSet facets;
    /// Boundary facet length
    Array1D<Real> lengths;
    /// Boundary facet unit outward normal
    Array1D<DenseVector<Real, DIM>> normals;
};

} // namespace fe

} // namespace godzilla
