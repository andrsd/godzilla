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

inline const Index &
Element::get_id() const
{
    return id;
}

inline const uint &
Element::get_marker() const
{
    return marker;
}

} // namespace godzilla
