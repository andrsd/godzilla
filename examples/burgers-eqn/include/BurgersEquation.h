#pragma once

#include "godzilla/ExplicitFELinearProblem.h"

using namespace godzilla;

/// Solves Burgers equation using finite element method
///
class BurgersEquation : public ExplicitFELinearProblem {
public:
    explicit BurgersEquation(const Parameters & parameters);
    void create() override;

    const PetscReal & get_viscosity() const;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;

    PetscInt u_id;
    const PetscReal & viscosity;

public:
    static Parameters parameters();
};
