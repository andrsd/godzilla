#include "Edge.h"
#include "CallStack.h"

namespace godzilla {

Edge::Edge() : Element1D()
{
    _F_;
}

Edge::Edge(Index v[]) : Element1D()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Edge::Edge(Index v1, Index v2) : Element1D()
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

} // namespace godzilla
