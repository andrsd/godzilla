#include "Vertex.h"

namespace godzilla {

Vertex::Vertex()
{
    this->x = this->y = this->z = 0.0;
}

Vertex::Vertex(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vertex::Vertex(const Vertex & o)
{
    this->x = o.x;
    this->y = o.y;
    this->z = o.z;
}

Vertex *
Vertex::copy()
{
    return new Vertex(*this);
}

} // namespace godzilla
