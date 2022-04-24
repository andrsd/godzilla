#include "Quad.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Quad::Quad(PetscInt id) : Element2D(id)
{
    _F_;
}

Quad::Quad(PetscInt id, PetscInt v[]) : Element2D(id)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Quad::Quad(PetscInt id, PetscInt v1, PetscInt v2, PetscInt v3, PetscInt v4) : Element2D(id)
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
    this->vertices[3] = v4;
}

Quad::Quad(const Quad & o) : Element2D(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

uint
Quad::get_edge_orientation(uint edge) const
{
    _F_;
    error("Not implemented");
    return 0;
}

uint
Quad::get_face_orientation(uint face) const
{
    _F_;
    error("2D quadrilateral has no face functions");
    return 0;
}

} // namespace godzilla
