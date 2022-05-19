#pragma once

#include "NaturalBC.h"

namespace godzilla {

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const InputParameters & params);

    virtual PetscInt get_field_id() const;
    virtual PetscInt get_num_components() const;
    virtual std::vector<PetscInt> get_components() const;

protected:
    virtual void on_set_weak_form();

public:
    static InputParameters valid_params();
};

} // namespace godzilla
