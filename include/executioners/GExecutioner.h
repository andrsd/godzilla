#pragma once

#include "MooseObject.h"
#include "GPrintInterface.h"

class GProblem;

/// Executioner
///
class GExecutioner : public MooseObject,
                     public GPrintInterface
{
public:
    GExecutioner(const InputParameters & parameters);

    virtual void execute();

protected:
    GProblem & problem;

public:
    static InputParameters validParams();
};
