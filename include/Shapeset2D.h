#pragma once

#include "Shapeset.h"
#include "Common.h"
#include "Quadrature2D.h"

namespace godzilla {

// shape function in 2D
typedef Real (*shape_fn_2d_t)(Real, Real);

/// Base class for 2D shapesets
///
class Shapeset2D : public Shapeset {
public:
    Shapeset2D(EMode2D mode, uint n_components);
    virtual ~Shapeset2D();

    virtual uint * get_face_indices(uint face, uint ori, uint order) const;

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
