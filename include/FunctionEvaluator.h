#pragma once

#include "muParser.h"
#include "petsc.h"

namespace godzilla {

class Function;

/// Class to evaluate function expressions given by a string
///
class FunctionEvaluator {
public:
    FunctionEvaluator();

    /// Build the evaluator object
    ///
    /// @param expr Function expression to be evaluated
    /// @param storage Function storage with all external functions that can be used by the
    /// evaluator
    void create(const std::string & expr, const std::vector<Function *> & storage);

    /// Evaluate the function expression at time `time` and spatial position `x`
    ///
    /// @param dim Spatial dimension
    /// @param time Simulation time
    /// @param x Spatial location
    PetscReal evaluate(PetscInt dim, PetscReal time, const PetscReal x[]);

protected:
    /// Underlying muParser object
    mu::Parser parser;
    /// Function expression to be evaluated
    std::string fn_expression;
};

} // namespace godzilla
