#pragma once

#include "Function1D.h"
#include "Shapeset1D.h"
#include "Element.h"

namespace godzilla {

// Represents a shape function on a ref. domain
//
//
class ShapeFunction1D : public RealFunction1D {
public:
    /// Constructs a standard precalculated shapeset class.
    /// @param shapeset [in] Pointer to the shapeset to be precalculated.
    ShapeFunction1D(Shapeset1D * shapeset);
    ShapeFunction1D();

    /// Destructor.
    virtual ~ShapeFunction1D();

    void free();

    /// Ensures subsequent calls to get_active_element() will be returning 'e'.
    /// Switches the class to the appropriate mode (triangle, quad).
    virtual void set_active_element(Element1D * e);

    /// Activates a shape function given by its index. The values of the shape function
    /// can then be obtained by setting the required integration rule order by calling
    /// set_quad_order() and after that calling get_values(), get_dx_values(), etc.
    /// @param index [in] Shape index.
    void set_active_shape(uint index);

    /// @return Index of the active shape (can be negative if the shape is constrained).
    uint
    get_active_shape() const
    {
        return this->index;
    };

    /// @return Pointer to the shapeset which is being precalculated.
    Shapeset1D *
    get_shapeset() const
    {
        return this->shapeset;
    }

    void set_shapeset(Shapeset1D * ss);

    virtual void precalculate(const uint np, const QPoint1D * pt, uint mask);

protected:
    /// Spaheset used for evaluation
    Shapeset1D * shapeset;
    /// index of active shape function
    uint index;
};

} // namespace godzilla
