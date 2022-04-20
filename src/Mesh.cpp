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
Mesh::get_num_elements() const
{
    _F_;
    return this->elements.count();
}

void
Mesh::set_vertex(const Index & id, const Vertex * vertex)
{
    _F_;
    this->vertices[id] = vertex;
}

const Vertex *
Mesh::get_vertex(const Index & id) const
{
    return this->vertices[id];
}

void
Mesh::set_element(const Index & id, const Element * elem)
{
    _F_;
    this->elements[id] = elem;
}

} // namespace godzilla
