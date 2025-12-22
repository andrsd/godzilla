#pragma once

#include "godzilla/ExplicitFELinearProblem.h"
#include "godzilla/Types.h"

using namespace godzilla;

/// Solves Burgers equation using finite element method
///
class BurgersEquation : public ExplicitFELinearProblem {
public:
    explicit BurgersEquation(const Parameters & pars);
    void create() override;

    const Real & get_viscosity() const;

protected:
    void set_up_fields() override;
    void set_up_weak_form() override;
    void set_up_time_scheme() override;

    FieldID u_id;
    const Real viscosity;

public:
    static Parameters parameters();
};
