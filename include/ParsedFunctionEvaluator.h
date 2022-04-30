#pragma once

#include "GodzillaConfig.h"
#include "muParser.h"

namespace godzilla {

class ParsedFunction;

/// Class to evaluate function expressions given by a string
///
class ParsedFunctionEvaluator {
public:
    ParsedFunctionEvaluator();

    /// Build the evaluator object
    ///
    /// @param expr Function expression to be evaluated
    /// @param storage Function storage with all external functions that can be used by the
    /// evaluator
    void create(const std::string & expr);

    /// Evaluate the function expression at time `time` and spatial position `x`
    ///
    /// @param dim Spatial dimension
    /// @param time Simulation time
    /// @param x x-coordinate
    /// @param y y-coordinate
    /// @param z z-coordinate
    Real evaluate(uint dim, Real time, Real x, Real y, Real z);

protected:
    /// Underlying muParser object
    mu::Parser parser;
    /// Function expression to be evaluated
    std::string fn_expression;
};

} // namespace godzilla
