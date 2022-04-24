#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents hexahedron in 3D
///
class Hex : public Element3D {
public:
    Hex(PetscInt id);
    Hex(PetscInt id, PetscInt v[]);
    Hex(PetscInt id,
        PetscInt v1,
        PetscInt v2,
        PetscInt v3,
        PetscInt v4,
        PetscInt v5,
        PetscInt v6,
        PetscInt v7,
        PetscInt v8);
    Hex(const Hex & o);

    virtual const DMPolytopeType
    get_potytope_type() const
    {
        return DM_POLYTOPE_HEXAHEDRON;
    }
    virtual uint
    get_num_vertices() const
    {
        return NUM_VERTICES;
    }
    virtual PetscInt
    get_vertex(uint local) const
    {
        return this->vertices[local];
    }
    virtual const PetscInt *
    get_vertices() const
    {
        return &(this->vertices[0]);
    }
    virtual uint
    get_num_edges() const
    {
        return NUM_EDGES;
    }
    virtual uint
    get_num_faces() const
    {
        return NUM_FACES;
    }

    virtual uint get_edge_orientation(uint edge) const;
    virtual uint get_face_orientation(uint face) const;

public:
    static const int NUM_VERTICES = 8;
    static const int NUM_FACES = 6;
    static const int NUM_EDGES = 12;

protected:
    /// Array of vertex indices that build up the hexahedron
    PetscInt vertices[NUM_VERTICES];
};

} // namespace godzilla
