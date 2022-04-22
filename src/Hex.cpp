#include "Hex.h"
#include "CallStack.h"
#include "Error.h"

namespace godzilla {

Hex::Hex() : Element3D()
{
    _F_;
}

Hex::Hex(Index v[]) : Element3D()
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = v[i];
}

Hex::Hex(Index v1, Index v2, Index v3, Index v4, Index v5, Index v6, Index v7, Index v8) : Element3D()
{
    _F_;
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
    this->vertices[3] = v4;
    this->vertices[4] = v5;
    this->vertices[5] = v6;
    this->vertices[6] = v7;
    this->vertices[7] = v8;
}

Hex::Hex(const Hex & o) : Element3D(o)
{
    _F_;
    for (uint i = 0; i < NUM_VERTICES; i++)
        this->vertices[i] = o.vertices[i];
}

uint
Hex::get_edge_orientation(uint edge) const
{
    _F_;
    error("Not implemented");
    return 0;
}

uint
Hex::get_face_orientation(uint face) const
{
    _F_;
    error("Not implemented");
    return 0;
}

} // namespace godzilla
