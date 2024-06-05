// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/EssentialBC.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

/// Dirichlet boundary condition
///
/// Can be used only on single-field problems
class DirichletBC : public EssentialBC, protected FunctionInterface {
public:
    explicit DirichletBC(const Parameters & params);

    void create() override;
    void set_up() override;
    const std::vector<Int> & get_components() const override;
    void evaluate(Real time, const Real x[], Scalar u[]) override;
    void evaluate_t(Real time, const Real x[], Scalar u[]) override;

private:
    std::vector<Int> components;

public:
    static Parameters parameters();
};

} // namespace godzilla
