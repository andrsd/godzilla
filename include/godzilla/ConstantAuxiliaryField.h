#pragma once

#include "godzilla/GodzillaConfig.h"
#include "godzilla/Types.h"
#include "godzilla/AuxiliaryField.h"

namespace godzilla {

/// Constant auxiliary field
///
class ConstantAuxiliaryField : public AuxiliaryField {
public:
    explicit ConstantAuxiliaryField(const Parameters & params);

    void create() override;
    NO_DISCARD Int get_num_components() const override;
    NO_DISCARD PetscFunc * get_func() const override;
    void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override;

protected:
    const std::vector<Real> & values;
    unsigned int num_comps;

public:
    static Parameters parameters();
};

} // namespace godzilla
