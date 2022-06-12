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
    void create(const std::string & expr);

    /// Define a constant
    ///
    /// @param name Name of the constant
    /// @param value Value
    void define_constant(const std::string & name, PetscReal value);

    /// Register user function with function evaluator
    ///
    /// @param fn Fuction to register
    void register_function(Function * fn);

    /// Evaluate the function expression at time `time` and spatial position `x`
    ///
    /// @param dim Spatial dimension
    /// @param time Simulation time
    /// @param x Spatial location
    PetscReal evaluate(PetscInt dim, PetscReal time, const PetscReal x[]);

    bool evaluate(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt nc, PetscReal u[]);

protected:
    /// Underlying muParser object
    mu::Parser parser;
    /// Function expression to be evaluated
    std::string fn_expression;
};

} // namespace godzilla
