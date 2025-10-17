#pragma once

#include "godzilla/NaturalBC.h"

using namespace godzilla;

class ConvectiveHeatFluxBC : public NaturalBC {
public:
    ConvectiveHeatFluxBC(const Parameters & pars);

    const std::vector<Int> & get_components() const override;

protected:
    void set_up_weak_form() override;

    std::vector<Int> components;

public:
    static Parameters parameters();
};
