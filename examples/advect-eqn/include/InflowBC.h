#pragma once

#include "godzilla/NaturalRiemannBC.h"

using namespace godzilla;

/// Inflow boundary condition
///
class InflowBC : public NaturalRiemannBC {
public:
    InflowBC(const Parameters & pars);

    void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override;

protected:
    /// Inlet velocity
    const Real inlet_vel;

public:
    static Parameters parameters();
};
