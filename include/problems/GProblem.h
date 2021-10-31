#pragma once

#include "problems/Problem.h"
#include "base/GPrintInterface.h"

/// Problem
///
class GProblem : public Problem,
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
