#pragma once

#include "Types.h"
#include "AuxiliaryField.h"
#include "FunctionInterface.h"

namespace godzilla {

/// Auxiliary field set by a function
///
class FunctionAuxiliaryField : public AuxiliaryField, public FunctionInterface {
public:
    explicit FunctionAuxiliaryField(const Parameters & params);

    void create() override;
    Int get_num_components() const override;
    PetscFunc * get_func() const override;
    void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
