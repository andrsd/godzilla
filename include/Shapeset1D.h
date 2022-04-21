#pragma once

#include "Common.h"
#include "Shapeset.h"
#include "Quadrature1D.h"
#include "Function1D.h"

namespace godzilla {

// shape function in 1D
typedef Real (*shape_fn_1d_t)(Real);

/// Base class for 1D shapesets
///
class Shapeset1D : public Shapeset {
public:
    Shapeset1D(EMode1D mode, uint n_components);
    virtual ~Shapeset1D();

    uint
    get_num_components() const
    {
        return num_components;
    }

    // @return index of a vertex shape function for a vertex
    // @param [in] vertex - index of the vertex
    virtual uint get_vertex_index(uint vertex) const = 0;

    /// @return indices of bubble functions
    /// @param order - order of the bubble function
    virtual uint * get_bubble_indices(uint order) const = 0;

    virtual uint get_num_bubble_fns(uint order) const = 0;

    /// Get order of s shape function with index `index`
    ///
    /// @param[in] index Index of the shape function
    /// @return Order of the shape function
    virtual uint get_order(uint index) const = 0;

    /// Evaluate function in the set of points
    /// @param[in] n Type of the value
    /// @param[in] index - Index of the function being evaluate
    /// @param[in] np - The number of points in array 'pt'
    /// @param[in] pt - Points where the function is evaluated
    /// @param[in] component - The number of component of the evaluated function
    /// @param[out] vals - The array of vakues (caller is responsible for freeing this memory)
    virtual void
    get_values(uint n, uint index, uint np, const QPoint1D * pt, uint component, Real * vals) const = 0;

    /// Evaluate function 'index' in points 'pt'
    virtual Real get_value(uint n, uint index, Real x, uint component) const = 0;

    void
    get_fn_values(uint index, uint np, const QPoint1D * pt, uint component, Real * vals) const
    {
        get_values(RealFunction1D::FN, index, np, pt, component, vals);
    }

    void
    get_dx_values(uint index, uint np, const QPoint1D * pt, uint component, Real * vals) const
    {
        get_values(RealFunction1D::DX, index, np, pt, component, vals);
    }

    void
    get_dxx_values(uint index, uint np, const QPoint1D * pt, uint component, Real * vals) const
    {
        get_values(RealFunction1D::DXX, index, np, pt, component, vals);
    }

    Real
    get_fn_value(uint index, Real x, uint component) const
    {
        return get_value(RealFunction1D::FN, index, x, component);
    }

    Real
    get_dx_value(uint index, Real x, uint component) const
    {
        return get_value(RealFunction1D::DX, index, x, component);
    }

    Real
    get_dxx_value(uint index, Real x, uint component) const
    {
        return get_value(RealFunction1D::DXX, index, x, component);
    }

protected:
    /// Shapeset mode
    EMode1D mode;
    /// Number of components
    uint num_components;
};

} // namespace godzilla
