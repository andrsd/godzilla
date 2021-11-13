#pragma once

#include "Executioner.h"

namespace godzilla {

/// Executioner for steady-state simulations
///
class Steady : public Executioner {
public:
    Steady(const InputParameters & parameters);

    virtual void execute();

public:
    static InputParameters validParams();
};

} // namespace godzilla
