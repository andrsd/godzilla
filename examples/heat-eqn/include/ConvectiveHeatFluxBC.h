#pragma once

#include "NaturalBC.h"

namespace godzilla {

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const Parameters & params);

    virtual PetscInt get_num_components() const;
    virtual std::vector<PetscInt> get_components() const;

protected:
    virtual void set_up_weak_form();

public:
    static Parameters valid_params();
};

} // namespace godzilla
