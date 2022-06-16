#pragma once

#include "InitialCondition.h"
#include <vector>

namespace godzilla {

/// Constant initial condition
///
/// Use to set the initial condition of a field to constant values
/// Note that if the field has multiple components, you can provide a single
/// value for each component
class ConstantIC : public InitialCondition {
public:
    ConstantIC(const InputParameters & params);

    virtual PetscInt get_num_components() const override;

    virtual void evaluate(PetscInt dim,
                          PetscReal time,
                          const PetscReal x[],
                          PetscInt Nc,
                          PetscScalar u[]) override;

protected:
    /// Constant values -- one for each component
    const std::vector<PetscReal> & values;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
