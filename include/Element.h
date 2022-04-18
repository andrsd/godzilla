#pragma once

#include "GodzillaConfig.h"

namespace godzilla {

/// Base class for elements (abstract)
///
class Element {
public:
    Element();
    Element(const Element & o);

    /// Get element ID
    const Index & getId() const;
    /// Get element marker
    const uint & getMarker() const;
    /// Set element marker
    void setMarker(const uint & marker);

protected:
    /// Element ID
    Index id;
    /// Marker
    uint marker;
};

} // namespace godzilla
