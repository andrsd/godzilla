#pragma once

#include "BoundaryCondition.h"
#include "Label.h"
#include "Types.h"

namespace godzilla {

/// Base class for natural Riemann boundary conditions
class NaturalRiemannBC : public BoundaryCondition {
public:
    explicit NaturalRiemannBC(const Parameters & params);

    void create() override;
    void set_up() override;

    /// Get the label this boundary condition operates on
    ///
    /// @return The label
    NO_DISCARD virtual const godzilla::Label & get_label() const;

    /// Get the label values this boundary condition operates on
    ///
    /// @return The label values
    NO_DISCARD virtual const std::vector<godzilla::Int> & get_ids() const;

    /// Get the ID of the field this boundary condition operates on
    ///
    /// @return ID of the field
    NO_DISCARD virtual Int get_field_id() const;

    /// Get the component numbers this boundary condition is constraining
    ///
    /// @return Vector of component numbers
    NO_DISCARD virtual const std::vector<Int> & get_components() const = 0;

    /// Evaluate the boundary condition
    ///
    /// @param time The time at which to sample
    /// @param c Centroid
    /// @param n Normal vector
    /// @param xI State at the interior cell
    /// @param xG State at the ghost cell (computed)
    virtual void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) = 0;

protected:
    /// DMLabel associated with the boundary name this boundary condition acts on
    godzilla::Label label;
    /// IDs of the label
    std::vector<godzilla::Int> ids;
    /// Field ID this boundary condition is attached to
    Int fid;

public:
    static Parameters parameters();
};

} // namespace godzilla
