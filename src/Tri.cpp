#include "Tri.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Tri::Tri(PetscInt id) : Element2D(id)
{
    _F_;
}

Tri::Tri(PetscInt id, PetscInt v[]) : Element2D(id)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Tri::Tri(PetscInt id, PetscInt v1, PetscInt v2, PetscInt v3) : Element2D(id)
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
}

Tri::Tri(const Tri & o) : Element2D(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

uint
Tri::get_edge_orientation(uint edge) const
{
    _F_;
    error("Not implemented");
    return 0;
}

uint
Tri::get_face_orientation(uint face) const
{
    _F_;
    error("2D triangle has no face functions");
    return 0;
}

} // namespace godzilla
