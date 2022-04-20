#include "Tetra.h"
#include "CallStack.h"

namespace godzilla {

Tetra::Tetra() : Element3D()
{
    _F_;
}

Tetra::Tetra(Index v[]) : Element3D()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Tetra::Tetra(Index v1, Index v2, Index v3, Index v4) : Element3D()
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

} // namespace godzilla
