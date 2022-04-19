#pragma once

#include "Common.h"

namespace godzilla {

/// Reference domain for a 1D edge
///
class RefEdge {
public:
    static const Point1D *
    get_vertices()
    {
        return vertices;
    }

protected:
    static const Point1D vertices[];
};

/// Reference domain for a 2D triangle
///
class RefTri {
public:
    static const Point2D *
    get_vertices()
    {
        return vertices;
    }

    static const uint *
    get_edge_vertices(uint edge)
    {
        return edge_vtcs[edge];
    }

protected:
    static const Point2D vertices[];
    static const uint2 edge_vtcs[];
};

/// Reference domain for a 2D quadrilateral
///
class RefQuad {
public:
    static const Point2D *
    get_vertices()
    {
        return vertices;
    }

    static const uint *
    get_edge_vertices(uint edge)
    {
        return edge_vtcs[edge];
    }

protected:
    static const Point2D vertices[];
    static const uint2 edge_vtcs[];
};

/// Reference domain for 3D tetrahedron
///
class RefTetra {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const uint *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

    static uint
    get_edge_orientations()
    {
        return 2;
    }

    static uint
    get_num_face_vertices(uint face)
    {
        return face_nvtcs[face];
    }

    static uint
    get_num_face_edges(uint face)
    {
        return face_nedges[face];
    }

    static const uint *
    get_face_vertices(uint face)
    {
        return face_vtcs[face];
    }

    static const uint *
    get_face_edges(int face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(uint face)
    {
        return face_mode[face];
    }

    static uint
    get_face_orientations(uint face)
    {
        return face_orientations[face];
    }

    static const Point3D
    get_face_normal(uint iface)
    {
        return face_normal[iface];
    }

protected:
    static const Point3D vertices[];
    static const uint2 edge_vtcs[];
    static const uint * face_vtcs[];
    static const uint * face_edges[];
    static const uint face_nvtcs[];
    static const uint face_nedges[];
    static const EMode2D face_mode[];
    static const uint face_orientations[];

    static const Point3D face_normal[];
};

/// Reference domain for a 3D hexahedron
class RefHex {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const uint *
    get_edge_vertices(uint edge)
    {
        return edge_vtcs[edge];
    }

    static uint
    get_edge_orientations()
    {
        return 2;
    }

    static uint
    get_num_face_vertices(uint face)
    {
        return face_nvtcs[face];
    }

    static uint
    get_num_face_edges(uint face)
    {
        return face_nedges[face];
    }

    static const uint *
    get_face_vertices(uint face)
    {
        return face_vtcs[face];
    }

    static const uint *
    get_face_edges(uint face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(uint face)
    {
        return face_mode[face];
    }

    static uint
    get_face_orientations(uint face)
    {
        return face_orientations[face];
    }

    static uint
    get_edge_tangent_direction(uint edge)
    {
        return edge_tangent[edge];
    }

    static uint
    get_face_tangent_direction(uint face, uint which)
    {
        return face_tangent[face][which];
    }

protected:
    static const Point3D vertices[];
    static const uint2 edge_vtcs[];
    static const uint * face_vtcs[];
    static const uint * face_edges[];
    static const uint face_nvtcs[];
    static const uint face_nedges[];
    static const EMode2D face_mode[];
    static const uint face_orientations[];
    static const uint edge_tangent[];
    static const uint2 face_tangent[];
};

/// Reference domain for a 3D prism
///
class RefPrism {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const uint *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

    static uint
    get_edge_orientations()
    {
        return 2;
    }

    static uint
    get_num_face_vertices(uint face)
    {
        return face_nvtcs[face];
    }

    static uint
    get_num_face_edges(uint face)
    {
        return face_nedges[face];
    }

    static const uint *
    get_face_vertices(uint face)
    {
        return face_vtcs[face];
    }

    static const uint *
    get_face_edges(uint face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(uint face)
    {
        return face_mode[face];
    }

    static uint
    get_face_orientations(uint face)
    {
        return face_orientations[face];
    }

protected:
    static const Point3D vertices[];
    static const uint2 edge_vtcs[];
    static const uint * face_vtcs[];
    static const uint * face_edges[];
    static const uint face_nvtcs[];
    static const uint face_nedges[];
    static const EMode2D face_mode[];
    static const uint face_orientations[];
};

} // namespace godzilla
