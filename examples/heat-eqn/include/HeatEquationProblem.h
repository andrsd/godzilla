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
    virtual void onSetFields() override;
    virtual void onSetWeakForm() override;

    /// ID for the 'temperature' field
    const PetscInt itemp;

public:
    static InputParameters validParams();
};
