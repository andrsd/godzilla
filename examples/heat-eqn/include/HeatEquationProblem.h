#pragma once

#include "ImplicitFENonlinearProblem.h"

using namespace godzilla;

/// Heat equation solver
///
class HeatEquationProblem : public ImplicitFENonlinearProblem {
public:
    HeatEquationProblem(const InputParameters & parameters);
    virtual ~HeatEquationProblem();

protected:
    virtual void on_set_fields() override;
    virtual void on_set_weak_form() override;

    /// ID for the 'temperature' field
    const PetscInt itemp;

public:
    static InputParameters valid_params();

    static const PetscInt htc_aux_id = 0;
    static const PetscInt T_ambient_aux_id = 1;
};
