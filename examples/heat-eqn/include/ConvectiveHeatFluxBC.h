#pragma once

#include "godzilla/NaturalBC.h"

class ConvectiveHeatFluxBC : public godzilla::NaturalBC {
public:
    ConvectiveHeatFluxBC(const godzilla::Parameters & pars);

protected:
    std::vector<godzilla::Int> create_components() override;
    void set_up_weak_form() override;

public:
    static godzilla::Parameters parameters();
};
