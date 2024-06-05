// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Auxiliary field set by a function
///
class FunctionAuxiliaryField : public AuxiliaryField, protected FunctionInterface {
public:
    explicit FunctionAuxiliaryField(const Parameters & params);

    void create() override;
    Int get_num_components() const override;
    void evaluate(Real time, const Real x[], Scalar u[]) override;

public:
    static Parameters parameters();
};

} // namespace godzilla
