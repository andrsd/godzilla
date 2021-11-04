#pragma once

#include "base/Object.h"
#include "base/GPrintInterface.h"

namespace godzilla {

/// Problem
///
class GProblem : public Object,
                 public GPrintInterface
{
public:
    GProblem(const InputParameters & parameters);

    /// Build the problem to solve
    virtual void create() = 0;
    virtual void init();
    /// Solve the problem
    virtual void solve() = 0;
    /// true if solve converged, otherwise false
    virtual bool converged() = 0;
    /// output
    virtual void out() = 0;

public:
    static InputParameters validParams();
};

}
