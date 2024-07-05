// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Postprocessor.h"
#include "godzilla/FunctionInterface.h"

namespace godzilla {

class FEProblemInterface;
class ParsedFunction;

/// Computes the L_2 difference between a function `u` and an FEM interpolant solution `u_h`,
/// separated into field components.
///
class L2FieldDiff : public Postprocessor {
public:
    explicit L2FieldDiff(const Parameters & params);

    void create() override;
    void compute() override;
    Real get_value() override;

private:
    /// FE problem
    const FEProblemInterface * fepi;

    /// Parsed functions associated with each field
    std::map<std::string, ParsedFunction *> parsed_fns;

    /// Computed L_2 errors
    std::vector<Real> l2_diff;

public:
    static Parameters parameters();
};

} // namespace godzilla
