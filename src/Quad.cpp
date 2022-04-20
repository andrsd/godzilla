#include "Quad.h"
#include "CallStack.h"

namespace godzilla {

Quad::Quad() : Element2D()
{
    _F_;
}

Quad::Quad(Index v[]) : Element2D()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Quad::Quad(Index v1, Index v2, Index v3, Index v4) : Element2D()
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

} // namespace godzilla
