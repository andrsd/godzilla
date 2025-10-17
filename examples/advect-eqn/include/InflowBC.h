#pragma once

#include "godzilla/NaturalRiemannBC.h"

using namespace godzilla;

/// Inflow boundary condition
///
class InflowBC : public NaturalRiemannBC {
public:
    InflowBC(const Parameters & pars);

    const std::vector<Int> & get_components() const override;
    void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override;

protected:
    /// Inlet velocity
    const Real inlet_vel;
    std::vector<Int> components;

public:
    static Parameters parameters();
};
