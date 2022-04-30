#pragma once

#include "GodzillaConfig.h"
#include "InputParameters.h"
#include "ParsedFunctionEvaluator.h"

namespace godzilla {

/// Interface for parsed function
///
class ParsedFunctionInterface {
public:
    ParsedFunctionInterface(const InputParameters & params);

    /// Build the evaluator
    void create();

    /// Evaluate parsed function
    ///
    /// @return the result of evaluation
    /// @param idx The index of the parsed expression. Ranges from 0..`num_comps`
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x, y, z Spatial coordinates where we evaluate the function
    Real evaluateFunction(uint idx, uint dim, Real time, Real x, Real y, Real z);

protected:
    /// Reference to the application
    const App & pfi_app;
    /// Function expressions
    const std::vector<std::string> & expression;
    /// Number of parsed function expressions
    uint num_comps;
    /// Function evaluators
    std::vector<ParsedFunctionEvaluator> evalr;

public:
    static InputParameters validParams();
};

} // namespace godzilla
