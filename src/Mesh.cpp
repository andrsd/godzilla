#include "Godzilla.h"
#include "Mesh.h"
#include "CallStack.h"
#include "Section.h"

namespace godzilla {

Parameters
Mesh::parameters()
{
    Parameters params = Object::parameters();
    return params;
}

Mesh::Mesh(const Parameters & parameters) : Object(parameters), PrintInterface(this), dim(-1) {}

Int
Mesh::get_dimension() const
{
    _F_;
    return this->dim;
}

void
Mesh::set_local_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetLocalSection(get_dm(), section));
}

void
Mesh::set_global_section(const Section & section) const
{
    _F_;
    PETSC_CHECK(DMSetGlobalSection(get_dm(), section));
}

} // namespace godzilla
