#pragma once

#include "Common.h"
#include "Quadrature2D.h"

namespace godzilla {

// shape function in 2D
typedef Real (*shape_fn_2d_t)(Real, Real);

/// Base class for 2D shapesets
///
class Shapeset2D {
public:
    Shapeset2D(EMode2D mode, uint n_components);
    virtual ~Shapeset2D();

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

    /// @return indices of bubble functions
    /// @param order - order of the bubble function
    virtual uint * get_bubble_indices(uint order) = 0;

    virtual uint get_num_edge_fns(uint order) const = 0;

    virtual uint get_num_bubble_fns(uint order) const = 0;

    /// Get the number of possible orientations on an edge
    ///
    /// @return The number of possbile orientations on an edge
    virtual uint get_edge_orientations() const = 0;

    virtual uint get_order(uint index) const = 0;

    virtual int get_shape_type(uint index) const = 0;

    /// Evaluate function in the set of points
    /// @param[in] n
    /// @param[in] index - Index of the function being evaluate
    /// @param[in] np - The number of points in array 'pt'
    /// @param[in] pt - Points where the function is evaluated
    /// @param[in] component - The number of component of the evaluated function
    /// @param[out] vals - The array of vakues (caller is responsible for freeing this memory)
    virtual void
    get_values(EValueType n, uint index, uint np, QPoint2D * pt, uint component, Real * vals) = 0;

    /// Evaluate function 'index' in points 'pt'
    virtual Real get_value(EValueType n, uint index, Real x, Real y, uint component) = 0;

    void
    get_fn_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(FN, index, np, pt, component, vals);
    }

    void
    get_dx_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(DX, index, np, pt, component, vals);
    }

    void
    get_dy_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(DY, index, np, pt, component, vals);
    }

    void
    get_dxx_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(DXX, index, np, pt, component, vals);
    }

    void
    get_dyy_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(DYY, index, np, pt, component, vals);
    }

    void
    get_dxy_values(uint index, uint np, QPoint2D * pt, uint component, Real * vals)
    {
        get_values(DXY, index, np, pt, component, vals);
    }

    Real
    get_fn_value(uint index, Real x, Real y, uint component)
    {
        return get_value(FN, index, x, y, component);
    }

    Real
    get_dx_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DX, index, x, y, component);
    }

    Real
    get_dy_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DY, index, x, y, component);
    }

    Real
    get_dz_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DZ, index, x, y, component);
    }

    Real
    get_dxx_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DXX, index, x, y, component);
    }

    Real
    get_dyy_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DYY, index, x, y, component);
    }

    Real
    get_dxy_value(uint index, Real x, Real y, uint component)
    {
        return get_value(DXY, index, x, y, component);
    }

protected:
    /// Shapeset mode
    EMode2D mode;
    /// Number of components
    uint num_components;
};

} // namespace godzilla
