#include "Tri.h"
#include "CallStack.h"

namespace godzilla {

Tri::Tri() : Element()
{
    _F_;
}

Tri::Tri(Index v[]) : Element()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Tri::Tri(Index v1, Index v2, Index v3) : Element()
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
}

Tri::Tri(const Tri & o) : Element(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

} // namespace godzilla
