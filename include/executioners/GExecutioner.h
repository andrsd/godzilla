#pragma once

#include "base/MooseObject.h"
#include "base/GPrintInterface.h"

class GProblem;

/// Executioner
///
class GExecutioner : public MooseObject,
                     public GPrintInterface
{
public:
    GExecutioner(const InputParameters & parameters);

    virtual void create();
    virtual void execute();

protected:
    GProblem & problem;

public:
    static InputParameters validParams();
};
