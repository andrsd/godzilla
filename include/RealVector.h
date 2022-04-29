#pragma once

namespace godzilla {

// Real-valued vector in 1D
//
class RealVector1D {
public:
    RealVector1D() { this->val[0] = 0.; }
    RealVector1D(Real x) { this->val[0] = x; }

    const Real &
    operator[](uint idx) const
    {
        return this->val[idx];
    }

    Real &
    operator[](uint idx)
    {
        return this->val[idx];
    }

    Real
    operator*(const RealVector1D & o) const
    {
        return this->val[0] * o.val[0];
    }

protected:
    Real val[1];
};

// Real-valued vector in 2D
//
class RealVector2D {
public:
    RealVector2D()
    {
        this->val[0] = 0.;
        this->val[1] = 0.;
    }

    RealVector2D(Real x, Real y)
    {
        this->val[0] = x;
        this->val[1] = y;
    }

    const Real &
    operator[](uint idx) const
    {
        return this->val[idx];
    }

    Real &
    operator[](uint idx)
    {
        return this->val[idx];
    }

    Real
    operator*(const RealVector2D & o) const
    {
        return this->val[0] * o.val[0] + this->val[1] * o.val[1];
    }

protected:
    Real val[2];
};

// Real-valued vector in 3D
//
class RealVector3D {
public:
    RealVector3D()
    {
        this->val[0] = 0.;
        this->val[1] = 0.;
        this->val[2] = 0.;
    }

    RealVector3D(Real x, Real y, Real z)
    {
        this->val[0] = x;
        this->val[1] = y;
        this->val[2] = z;
    }

    const Real &
    operator[](uint idx) const
    {
        return this->val[idx];
    }

    Real &
    operator[](uint idx)
    {
        return this->val[idx];
    }

    Real
    operator*(const RealVector3D & o) const
    {
        return this->val[0] * o.val[0] + this->val[1] * o.val[1] + this->val[2] * o.val[2];
    }

protected:
    Real val[3];
};

} // namespace godzilla
