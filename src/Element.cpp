#include "Element.h"
#include "CallStack.h"

namespace godzilla {

Element::Element()
{
    _F_;
    this->id = INVALID_IDX;
    this->marker = 0;
}

Element::Element(const Element & o)
{
    _F_;
    this->id = o.id;
    this->marker = o.marker;
}

const Index &
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
