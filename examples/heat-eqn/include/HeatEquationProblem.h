#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class HeatEquationProblem : public ImplicitFENonlinearProblem {
public:
    HeatEquationProblem(const Parameters & parameters);
    virtual ~HeatEquationProblem();

protected:
    virtual void set_up_fields() override;
    virtual void set_up_weak_form() override;

    /// Polynomial order of the FE space
    const PetscInt & p_order;

public:
    static Parameters valid_params();

    static const PetscInt temp_id = 0;

    static const PetscInt q_ppp_id = 0;
    static const PetscInt htc_aux_id = 1;
    static const PetscInt T_ambient_aux_id = 2;
};
