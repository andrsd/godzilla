#pragma once

#include "petsc.h"
#include "InputParameters.h"
#include "FunctionEvaluator.h"

namespace godzilla {

class App;

/// Interface for parsed function
///
class FunctionInterface {
public:
    FunctionInterface(const InputParameters & params);

    /// Build the evaluator
    void create();

    /// Evaluate parsed function
    ///
    /// @return the result of evaluation
    /// @param idx The index of the parsed expression. Ranges from 0..`num_comps`
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x Spatial coordinate where we evaluate the function (has size of `dim`)
    PetscReal evaluate(unsigned int idx, PetscInt dim, PetscReal time, const PetscReal x[]);

    /// Evaluate parsed time derivative function
    ///
    /// @return the result of evaluation
    /// @param idx The index of the parsed expression. Ranges from 0..`num_comps`
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x Spatial coordinate where we evaluate the function (has size of `dim`)
    PetscReal evaluate_t(unsigned int idx, PetscInt dim, PetscReal time, const PetscReal x[]);

protected:
    /// Application
    const App * fi_app;
    /// Function expressions
    const std::vector<std::string> & expression;
    /// Time derivatives of `expression`
    const std::vector<std::string> expression_t;
    /// Number of parsed function expressions
    unsigned int num_comps;
    /// Function evaluators
    std::vector<FunctionEvaluator> evalr;
    /// Time derivative evaluators
    std::vector<FunctionEvaluator> evalr_t;

public:
    static InputParameters valid_params();
    static InputParameters valid_params_t();
};

} // namespace godzilla
