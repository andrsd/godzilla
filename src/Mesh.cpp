#include "Mesh.h"
#include "CallStack.h"

namespace godzilla {

Mesh::Mesh()
{
    _F_;
}

Mesh::~Mesh()
{
    _F_;
    free();
}

void
Mesh::free()
{
    _F_;
}

uint
Mesh::getNumElements() const
{
    _F_;
    return this->elements.count();
}

void
Mesh::setVertex(const Index & id, const Vertex * vertex)
{
    _F_;
    this->vertices[id] = vertex;
}

void
Mesh::setElement(const Index & id, const Element * elem)
{
    _F_;
    this->elements[id] = elem;
}

} // namespace godzilla
