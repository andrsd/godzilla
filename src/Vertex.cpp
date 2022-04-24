#include "Vertex.h"

namespace godzilla {

// Vertex1D

Vertex1D::Vertex1D(PetscInt id) : Vertex(id)
{
    this->x = 0.0;
}

Vertex1D::Vertex1D(PetscInt id, double x) : Vertex(id)
{
    this->x = x;
}

Vertex1D::Vertex1D(const Vertex1D & o) : Vertex(o)
{
    this->x = o.x;
}

Vertex1D *
Vertex1D::copy()
{
    return new Vertex1D(*this);
}

// Vertex2D

Vertex2D::Vertex2D(PetscInt id) : Vertex(id)
{
    this->x = this->y = 0.0;
}

Vertex2D::Vertex2D(PetscInt id, double x, double y) : Vertex(id)
{
    this->x = x;
    this->y = y;
}

Vertex2D::Vertex2D(const Vertex2D & o) : Vertex(o)
{
    this->x = o.x;
    this->y = o.y;
}

Vertex2D *
Vertex2D::copy()
{
    return new Vertex2D(*this);
}

// Vertex3D

Vertex3D::Vertex3D(PetscInt id) : Vertex(id)
{
    this->x = this->y = this->z = 0.0;
}

Vertex3D::Vertex3D(PetscInt id, double x, double y, double z) : Vertex(id)
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
