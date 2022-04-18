#include "Edge.h"
#include "CallStack.h"

namespace godzilla {

Edge::Edge() : Element()
{
    _F_;
}

Edge::Edge(Index v[]) : Element()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Edge::Edge(Index v1, Index v2) : Element()
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
}

Edge::Edge(const Edge & o) : Element(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

} // namespace godzilla
