// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Function.h"
#include "godzilla/FunctionEvaluator.h"
#include "muParser/muParser.h"
#include "godzilla/Types.h"

namespace godzilla {

/// Parsed function
///
class ParsedFunction : public Function {
public:
    explicit ParsedFunction(const Parameters & params);

    /// Register this function with the function parser
    ///
    /// @param parser The mu::Parser object we register this function with
    void register_callback(mu::Parser & parser) override;

    /// Evaluate the function
    ///
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param u The computed values
    virtual void evaluate(Real time, const Real x[], Scalar u[]);

private:
    /// Text representation of the function to evaluate (one per component)
    const std::vector<std::string> & function;
    /// User defined constants
    const std::map<std::string, Real> & constants;
    /// Function evaluator
    FunctionEvaluator evalr;

public:
    static Parameters parameters();
};

} // namespace godzilla
