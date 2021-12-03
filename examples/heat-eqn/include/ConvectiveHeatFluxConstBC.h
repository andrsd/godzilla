#pragma once

#include "NaturalBC.h"

namespace godzilla {

class ConvectiveHeatFluxConstBC : public NaturalBC {
public:
    ConvectiveHeatFluxConstBC(const InputParameters & params);

    virtual PetscInt getFieldId() const;
    virtual PetscInt getNumComponents() const;
    virtual std::vector<PetscInt> getComponents() const;

protected:
    virtual void onSetWeakForm();

    /// Convective heat transfer coefficient
    const PetscReal & htc;

    /// Ambient temperature
    const PetscReal & T_infinity;

public:
    static InputParameters validParams();
};

} // namespace godzilla
