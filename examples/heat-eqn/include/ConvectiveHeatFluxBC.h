#pragma once

#include "godzilla/NaturalBC.h"

using namespace godzilla;

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const Parameters & pars);

protected:
    std::vector<Int> create_components() override;
    void set_up_weak_form() override;

public:
    static Parameters parameters();
};
