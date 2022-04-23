#pragma once

#include "Common.h"
#include "Quadrature1D.h"
#include "RefMap1D.h"

namespace godzilla {

// Function
template <typename T>
class Fn1D {
public:
    /// number of components
    uint nc;
    /// function values
    T * fn;
    /// derivatives
    T * dx;

    Fn1D()
    {
        this->fn = nullptr;
        this->dx = nullptr;
    }
};

/// used for transformed shape functions
typedef Fn1D<Real> SFn1D;
/// used for transformed mesh functions
typedef Fn1D<Scalar> MFn1D;

/// Geometry of the element in 1D used in Residual/Jacobian forms
class Geom1D {
public:
    /// Init element geometry for volumetric integration
    Geom1D(uint marker, RefMap1D * rm, const uint np, const QPoint1D * pt)
    {
        this->marker = marker;
        this->x = rm->get_phys_x(np, pt);
    }

    /// Init element geometry for surface integration
    Geom1D(uint marker, RefMap1D * rm, uint iface, const uint np, const QPoint1D * pt)
    {
        this->marker = marker;
        this->x = rm->get_phys_x(np, pt);
        // TODO: calc normals
    }

    virtual ~Geom1D()
    {
        delete[] this->x;
        delete[] this->nx;
    }

public:
    /// Element/boundary marker
    uint marker;
    /// coordinates
    Real * x;
    /// normals
    Real * nx;
};

//

/// User defined data associated with Residual/Jacobian forms.
///
/// It holds arbitraty number of functions. Typically, these functions are solutions from
/// the previous time levels.
class UserData {
public:
    UserData()
    {
        this->nf = 0;
        this->ext = nullptr;
    }
    // TODO: ctor like UserData(uint nf, ...)

    ~UserData() { delete[] this->ext; }

protected:
    /// number of functions in 'fn' array
    uint nf;
    /// array of pointers to functions
    Fn1D<Scalar> * ext;
};

} // namespace godzilla
