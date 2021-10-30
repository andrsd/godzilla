#pragma once

#include "Problem.h"
#include "GPrintInterface.h"

class GMesh;

/// Problem
///
class GProblem : public Problem,
                 public GPrintInterface
{
public:
    GProblem(const InputParameters & parameters);

    virtual void init();
    virtual void solve();
    virtual bool converged();

protected:
    GMesh & _mesh;

public:
    static InputParameters validParams();
};
