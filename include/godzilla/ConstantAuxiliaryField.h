#pragma once

#include "godzilla/Types.h"
#include "godzilla/AuxiliaryField.h"

namespace godzilla {

/// Constant auxiliary field
///
class ConstantAuxiliaryField : public AuxiliaryField {
public:
    explicit ConstantAuxiliaryField(const Parameters & params);

    [[nodiscard]] Int get_num_components() const override;
    [[nodiscard]] PetscFunc * get_func() const override;
    void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override;

private:
    /// Values (one per component)
    const std::vector<Real> & values;

public:
    static Parameters parameters();
};

} // namespace godzilla
