#pragma once

#include "NaturalBC.h"

using namespace godzilla;

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const Parameters & params);

    virtual const std::vector<PetscInt> & get_components() const;

protected:
    virtual void set_up_weak_form();

    std::vector<PetscInt> components;

public:
    static Parameters parameters();
};
