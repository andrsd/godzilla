#pragma once

#include "MooseObject.h"

class GExecutioner : public MooseObject
{
public:
    GExecutioner(const InputParameters & parameters);

    virtual void execute() = 0;
};
