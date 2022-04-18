#include "Vertex.h"

namespace godzilla {

Vertex3D::Vertex3D() : Vertex()
{
    this->x = this->y = this->z = 0.0;
}

Vertex3D::Vertex3D(double x, double y, double z) : Vertex()
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vertex3D::Vertex3D(const Vertex3D & o) : Vertex(o)
{
    this->x = o.x;
    this->y = o.y;
    this->z = o.z;
}

Vertex3D *
Vertex3D::copy()
{
    return new Vertex3D(*this);
}

} // namespace godzilla
