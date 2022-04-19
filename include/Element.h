#pragma once

#include "Common.h"

namespace godzilla {

/// Base class for elements (abstract)
///
class Element {
public:
    Element();
    Element(const Element & o);

    /// Get element ID
    const Index & get_id() const;
    /// Get element marker
    const uint & get_marker() const;
    /// Set element marker
    void set_marker(const uint & marker);

protected:
    /// Element ID
    Index id;
    /// Marker
    uint marker;
};

} // namespace godzilla
