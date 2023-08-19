#pragma once

#include "NaturalBC.h"

using namespace godzilla;

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const Parameters & params);

    const std::vector<PetscInt> & get_components() const override;

protected:
    void set_up_weak_form() override;

    std::vector<PetscInt> components;

public:
    static Parameters parameters();
};
