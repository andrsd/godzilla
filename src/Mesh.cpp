#include "Godzilla.h"
#include "Mesh.h"
#include "CallStack.h"

namespace godzilla {

Parameters
Mesh::parameters()
{
    Parameters params = Object::parameters();
    return params;
}

Mesh::Mesh(const Parameters & parameters) : Object(parameters), PrintInterface(this), dim(-1) {}

PetscInt
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

} // namespace godzilla
