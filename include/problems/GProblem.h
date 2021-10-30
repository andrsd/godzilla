#pragma once

#include "problems/Problem.h"
#include "base/GPrintInterface.h"

class GMesh;

/// Problem
///
class GProblem : public Problem,
                 public GPrintInterface
{
public:
    GProblem(const InputParameters & parameters);

    virtual void create();
    virtual void init();
    virtual void solve();
    virtual bool converged();

protected:
    GMesh & mesh;

public:
    static InputParameters validParams();
};
