#include "Element.h"
#include "CallStack.h"

namespace godzilla {

Element::Element(PetscInt id)
{
    _F_;
    this->id = id;
    this->marker = 0;
}

Element::Element(const Element & o)
{
    _F_;
    this->id = o.id;
    this->marker = o.marker;
}

const PetscInt &
Element::get_id() const
{
    return id;
}

const uint &
Element::get_marker() const
{
    return marker;
}

void
Element::set_marker(const uint & marker)
{
    this->marker = marker;
}

} // namespace godzilla
