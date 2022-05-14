#pragma once

#include "NaturalBC.h"

namespace godzilla {

class ConvectiveHeatFluxConstBC : public NaturalBC {
public:
    ConvectiveHeatFluxConstBC(const InputParameters & params);

    virtual PetscInt get_field_id() const;
    virtual PetscInt get_num_components() const;
    virtual std::vector<PetscInt> get_components() const;

protected:
    virtual void on_set_weak_form();

    /// Convective heat transfer coefficient
    const PetscReal & htc;

    /// Ambient temperature
    const PetscReal & T_infinity;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
