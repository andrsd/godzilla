#pragma once

#include <math.h>

namespace godzilla {

inline Real
det2(Real a11, Real a12, Real a21, Real a22)
{
    return a11 * a22 - a12 * a21;
}

inline Real
det3(Real a11, Real a12, Real a13, Real a21, Real a22, Real a23, Real a31, Real a32, Real a33)
{
    return a11 * det2(a22, a23, a32, a33) - a12 * det2(a21, a23, a31, a33) +
           a13 * det2(a21, a22, a31, a32);
}

inline Real
det4(Real a11,
     Real a12,
     Real a13,
     Real a14,
     Real a21,
     Real a22,
     Real a23,
     Real a24,
     Real a31,
     Real a32,
     Real a33,
     Real a34,
     Real a41,
     Real a42,
     Real a43,
     Real a44)
{
    return a11 * det3(a22, a23, a24, a32, a33, a34, a42, a43, a44) -
           a12 * det3(a21, a23, a24, a31, a33, a34, a41, a43, a44) +
           a13 * det3(a21, a22, a24, a31, a32, a34, a41, a42, a44) -
           a14 * det3(a21, a22, a23, a31, a32, a33, a41, a42, a43);
}

inline Real
det(const Real1x1 & m)
{
    return m[0][0];
}

inline Real
det(const Real2x2 & m)
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

inline Real
det(const Real3x3 & m)
{
    return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
           m[2][0] * m[1][1] * m[0][2] - m[2][1] * m[1][2] * m[0][0] - m[2][2] * m[1][0] * m[0][1];
}

inline Real
dot_product(const Point3D & a, const Point3D & b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Point3D
cross_product(const Point3D & a, const Point3D & b)
{
    Point3D r(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    return r;
}

inline Real
norm(const Point3D & pt)
{
    return sqrt(dot_product(pt, pt));
}

inline Point3D
normalize(const Point3D & pt)
{
    Real n = norm(pt);
    Point3D res(pt.x / n, pt.y / n, pt.z / n);
    return res;
}

} // namespace godzilla
