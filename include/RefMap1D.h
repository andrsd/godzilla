#pragma once

#include "Common.h"
#include "Mesh.h"
#include "Quadrature1D.h"
#include "ShapeFunction1D.h"

namespace godzilla {

/// Reference mapping (for evaluating integrals on a physical domain)
///
class RefMap1D {
public:
    RefMap1D(const Mesh * mesh);
    virtual ~RefMap1D();

    /// Initializes the reference map for the specified element.
    /// Must be called prior to using all other functions in the class.
    /// @param[in] e - The element we want to work with
    virtual void set_active_element(const Element1D * e);

    /// @return The increase in the integration order due to the reference map.
    uint
    get_ref_order() const
    {
        return ref_order;
    }

    /// @return The increase in the integration order due to the inverse reference map.
    uint
    get_inv_ref_order() const
    {
        return inv_ref_order;
    }

    /// @return The array of jacobians of the reference map precalculated at the points 'pt'.
    /// @param[in] np - The number of points
    /// @param[in] pt - Points where the jacobian should be calculated
    /// @param[in] trans - set to true if you want transformed values
    Real * get_jacobian(const uint np, const QPoint1D * pt, bool trans = true);

    /// @return The jacobi matrices of the reference map precalculated at the points 'pt'.
    /// @param[in] np - The number of points
    /// @param[in] pt - Points for which we want the jacobi matrices
    Real1x1 * get_ref_map(const uint np, const QPoint1D * pt);

    /// @return The transposed inverse matrices of the reference map precalculated at the points
    /// 'pt'.
    /// @param[in] np - The number of points
    /// @param[in] pt - Points for which we want the transposed jacobi matrices
    Real1x1 * get_inv_ref_map(const uint np, const QPoint1D * pt);

    /// @return The x-coordinates of the points transformed to the physical domain of the element
    /// @param[in] np - The number of points
    /// @param[in] pt - Points for which we want the x-coord
    Real * get_phys_x(const uint np, const QPoint1D * pt);

protected:
    const Mesh * mesh;
    /// Active element
    const Element1D * element;
    ShapeFunction1D * pss;

    bool is_const_jacobian;
    Real const_jacobian;
    Real1x1 const_inv_ref_map;
    Real1x1 const_ref_map;

    uint ref_order;
    uint inv_ref_order;

    /// number of coeffs in 'indices' array
    int n_coefs;
    int indices[2];
    /// Coefficients
    const Vertex1D ** coefs;
    // 2 = max number of vertices
    const Vertex1D * vertex[2];

    void calc_const_inv_ref_map();
    Real calc_face_const_jacobian(uint face);
};

} // namespace godzilla
