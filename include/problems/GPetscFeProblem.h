#pragma once

#include "Problem.h"

class GPetscFeProblem : public Problem
{
public:
    GPetscFeProblem(const InputParameters & parameters);

    virtual void init();
    virtual void solve();
    virtual bool converged();

public:
    static InputParameters validParams();
};
