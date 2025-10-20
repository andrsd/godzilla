#pragma once

#include "godzilla/NaturalRiemannBC.h"

using namespace godzilla;

/// Outflow boundary condition
///
class OutflowBC : public NaturalRiemannBC {
public:
    OutflowBC(const Parameters & pars);

    void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override;

public:
    static Parameters parameters();
};
