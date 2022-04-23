#pragma once

#include "Common.h"
#include "Element.h"

namespace godzilla {

/// Represents tetrahedron
///
class Tetra : public Element3D {
public:
    Tetra();
    Tetra(PetscInt v[]);
    Tetra(PetscInt v1, PetscInt v2, PetscInt v3, PetscInt v4);
    Tetra(const Tetra & o);

    virtual const DMPolytopeType get_potytope_type() const { return DM_POLYTOPE_TETRAHEDRON; }
    virtual uint get_num_vertices() const { return NUM_VERTICES; }
    virtual PetscInt get_vertex(uint local) const { return this->vertices[local]; }
    virtual const PetscInt * get_vertices() const { return &(this->vertices[0]); }
    virtual uint get_num_edges() const { return NUM_EDGES; }
    virtual uint get_num_faces() const { return NUM_FACES; }

    virtual uint get_edge_orientation(uint edge) const;
    virtual uint get_face_orientation(uint face) const;

public:
    static const uint NUM_VERTICES = 4;
    static const uint NUM_FACES = 4;
    static const uint NUM_EDGES = 6;

protected:
    /// Array of vertex indices that build up the tetrahedron
    PetscInt vertices[NUM_VERTICES];
};

} // namespace godzilla
