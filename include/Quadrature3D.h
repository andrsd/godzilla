#pragma once

#include "Error.h"
#include "Array.h"

namespace godzilla {

/// Quadrature point in 3D
///
struct QPoint3D {
    /// x-coordinate
    double x;
    /// y-coordinate
    double y;
    /// z-coordinate
    double z;
    /// weight
    double w;

    QPoint3D() {}
    QPoint3D(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    double
    operator[](int idx) const
    {
        if (idx == 0)
            return this->x;
        else if (idx == 1)
            return this->y;
        else if (idx == 2)
            return this->z;
        else
            error("Index out of bounds");
    }
};

/// Numerical quadratures in 3D
///
class Quadrature3D {
public:
    virtual QPoint3D *
    get_points(const int & order)
    {
        return tables[order];
    }

    virtual int
    get_num_points(const int & order)
    {
        return np[order];
    }

    virtual QPoint3D *
    get_edge_points(int edge, const int & order)
    {
        return edge_tables[edge][order];
    }
    int
    get_edge_num_points(int edge, const int & order) const
    {
        return np_edge[order];
    }

    virtual QPoint3D *
    get_face_points(int face, const int & order)
    {
        return face_tables[face][order];
    }

    int
    get_face_num_points(int face, const int & order) const
    {
        return np_face[order];
    }

    int
    get_edge_max_order(int edge) const
    {
        return max_edge_order;
    }

    int
    get_face_max_order(int face) const
    {
        return max_face_order;
    }

    int
    get_max_order() const
    {
        return max_order;
    }

protected:
    /// maximal order for integration (interpretation depends on the mode)
    int max_edge_order;
    int max_face_order;
    int max_order;

    Array<QPoint3D *> tables;
    Array<QPoint3D *> * edge_tables;
    Array<QPoint3D *> * face_tables;
    Array<int> np;
    Array<int> np_edge;
    Array<int> np_face;
};

} // namespace godzilla
