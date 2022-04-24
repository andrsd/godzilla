#include "Tetra.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Tetra::Tetra(PetscInt id) : Element3D(id)
{
    _F_;
}

Tetra::Tetra(PetscInt id, PetscInt v[]) : Element3D(id)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Tetra::Tetra(PetscInt id, PetscInt v1, PetscInt v2, PetscInt v3, PetscInt v4) : Element3D(id)
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
    this->vertices[3] = v4;
}

Tetra::Tetra(const Tetra & o) : Element3D(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

uint
Tetra::get_edge_orientation(uint edge) const
{
    _F_;
    error("Not implemented");
    return 0;
}

uint
Tetra::get_face_orientation(uint face) const
{
    _F_;
    error("Not implemented");
    return 0;
}

} // namespace godzilla
