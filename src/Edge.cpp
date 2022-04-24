#include "Edge.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Edge::Edge(PetscInt id) : Element1D(id)
{
    _F_;
}

Edge::Edge(PetscInt id, PetscInt v[]) : Element1D(id)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Edge::Edge(PetscInt id, PetscInt v1, PetscInt v2) : Element1D(id)
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
}

Edge::Edge(const Edge & o) : Element1D(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

uint
Edge::get_edge_orientation(uint edge) const
{
    _F_;
    error("1D edge has no edge functions");
    return 0;
}

uint
Edge::get_face_orientation(uint face) const
{
    _F_;
    error("1D edge has no face functions");
    return 0;
}

} // namespace godzilla
