#pragma once

#include "Common.h"
#include "Quadrature3D.h"

namespace godzilla {

// shape function in 3D
typedef Real (*shape_fn_3d_t)(Real, Real, Real);

/// Base class for 3D shapesets
///
class Shapeset3D {
public:
    Shapeset3D(EMode3D mode, uint n_components);
    virtual ~Shapeset3D();

    uint
    get_num_components() const
    {
        return num_components;
    }

    // @return index of a vertex shape function for a vertex
    // @param [in] vertex - index of the vertex
    virtual uint get_vertex_index(uint vertex) const = 0;

    /// @return indices of edge shape functions
    /// @param [in] edge - edge number (local)
    /// @param [in] ori - orientation of the edge (0 or 1)
    /// @param [in] order - order on the edge
    virtual uint * get_edge_indices(uint edge, uint ori, uint order) = 0;

    /// @return indices of face shape functions
    /// @param [in] face - face number (local)
    /// @param [in] ori - orientation of the face
    /// @param [in] order - order on the face
    virtual uint * get_face_indices(uint face, uint ori, uint order) = 0;

    /// @return indices of bubble functions
    /// @param order - order of the bubble function
    virtual uint * get_bubble_indices(uint order) = 0;

    virtual uint get_num_edge_fns(uint order) const = 0;

    virtual uint get_num_face_fns(uint order) const = 0;

    virtual uint get_num_bubble_fns(uint order) const = 0;

    /// Get the number of possible orientations on a face
    ///
    /// @return The number of possbile orientations on a face
    /// @param[in] face The number of the face for which the orientations are evaluated
    virtual uint get_face_orientations(uint face) const = 0;

    /// Get the number of possible orientations on an edge
    ///
    /// @return The number of possbile orientations on an edge
    virtual uint get_edge_orientations() const = 0;

    /// Returns the variant of a face function
    /// It is needed only in Hcurl-derived shapesets where we need to distinguish between to
    /// variants of shape functions, other shapesets are returning 0
    virtual uint
    get_face_fn_variant(uint index) const
    {
        return 0;
    }

    virtual uint get_order(uint index) const = 0;

    virtual uint get_shape_type(uint index) const = 0;

    /// Evaluate function in the set of points
    /// @param[in] n
    /// @param[in] index - Index of the function being evaluate
    /// @param[in] np - The number of points in array 'pt'
    /// @param[in] pt - Points where the function is evaluated
    /// @param[in] component - The number of component of the evaluated function
    /// @param[out] vals - The array of vakues (caller is responsible for freeing this memory)
    virtual void
    get_values(EValueType n, uint index, uint np, QPoint3D * pt, uint component, Real * vals) = 0;

    /// Evaluate function 'index' in points 'pt'
    virtual Real get_value(EValueType n, uint index, Real x, Real y, Real z, uint component) = 0;

    void
    get_fn_values(uint index, uint np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(FN, index, np, pt, component, vals);
    }

    void
    get_dx_values(uint index, uint np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DX, index, np, pt, component, vals);
    }

    void
    get_dy_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DY, index, np, pt, component, vals);
    }

    void
    get_dz_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DZ, index, np, pt, component, vals);
    }

    void
    get_dxx_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DXX, index, np, pt, component, vals);
    }

    void
    get_dyy_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DYY, index, np, pt, component, vals);
    }

    void
    get_dzz_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DZZ, index, np, pt, component, vals);
    }

    void
    get_dxy_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DXY, index, np, pt, component, vals);
    }

    void
    get_dxz_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DXZ, index, np, pt, component, vals);
    }

    void
    get_dyz_values(uint index, int np, QPoint3D * pt, uint component, Real * vals)
    {
        get_values(DYZ, index, np, pt, component, vals);
    }

    Real
    get_fn_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(FN, index, x, y, z, component);
    }

    Real
    get_dx_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DX, index, x, y, z, component);
    }

    Real
    get_dy_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DY, index, x, y, z, component);
    }

    Real
    get_dz_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DZ, index, x, y, z, component);
    }

    Real
    get_dxx_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DXX, index, x, y, z, component);
    }

    Real
    get_dyy_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DYY, index, x, y, z, component);
    }

    Real
    get_dzz_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DZZ, index, x, y, z, component);
    }

    Real
    get_dxy_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DXZ, index, x, y, z, component);
    }

    Real
    get_dxz_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DXZ, index, x, y, z, component);
    }

    Real
    get_dyz_value(uint index, Real x, Real y, Real z, uint component)
    {
        return get_value(DYZ, index, x, y, z, component);
    }

protected:
    /// Shapeset mode
    EMode3D mode;
    /// Number of components
    uint num_components;
};

} // namespace godzilla
