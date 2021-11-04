#pragma once

#include "base/Object.h"
#include "base/GPrintInterface.h"


namespace godzilla {

class GProblem;

/// Executioner
///
class GExecutioner : public Object,
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

}
