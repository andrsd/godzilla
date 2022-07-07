#pragma once

#include "ExplicitFELinearProblem.h"

using namespace godzilla;

/// Solves Burgers equation using finite element method
///
class BurgersEquation : public ExplicitFELinearProblem {
public:
    BurgersEquation(const Parameters & parameters);
    virtual ~BurgersEquation();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    const PetscReal & viscosity;

public:
    static Parameters parameters();

    static const PetscInt u_id = 0;
};
