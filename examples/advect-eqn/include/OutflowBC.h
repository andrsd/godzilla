#pragma once

#include "godzilla/NaturalRiemannBC.h"

using namespace godzilla;

/// Outflow boundary condition
///
class OutflowBC : public NaturalRiemannBC {
public:
    OutflowBC(const Parameters & pars);

    const std::vector<Int> & get_components() const override;
    void
    evaluate(Real time, const Real * c, const Real * n, const Scalar * xI, Scalar * xG) override;

protected:
    std::vector<Int> components;

public:
    static Parameters parameters();
};
