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

const Element *
Mesh::get_element(const Index & id) const
{
    _F_;
    return this->elements[id];
}

void
Mesh::set_boundary(const Index & eid, const uint & local_side, const uint & marker)
{
    _F_;
    SideBoundary * bnd = new SideBoundary(eid, local_side, marker);
    this->boundaries.add(bnd);
}

} // namespace godzilla
