#pragma once

#include "petsc.h"
#include "Parameters.h"
#include "FunctionEvaluator.h"

namespace godzilla {

class App;

/// Interface for parsed function
///
class FunctionInterface {
public:
    explicit FunctionInterface(const Parameters & params);

    /// Build the evaluator
    void create();

    /// Evaluate parsed function
    ///
    /// @return 'true' if evaluation was successful, `false` otherwise
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x Spatial coordinate where we evaluate the function (has size of `dim`)
    /// @param nc Number of components
    /// @param u The result of evaluation (one per component)
    bool evaluate_func(Int dim, PetscReal time, const PetscReal x[], Int nc, PetscReal u[]);

    /// Evaluate parsed time derivative function
    ///
    /// @return 'true' if evaluation was successful, `false` otherwise
    /// @param dim Spatial dimension of the evaluated function
    /// @param time Time of the time-dependent function
    /// @param x Spatial coordinate where we evaluate the function (has size of `dim`)
    /// @param nc Number of components
    /// @param u The result of evaluation (one per component)
    bool evaluate_func_t(Int dim, PetscReal time, const PetscReal x[], Int nc, PetscReal u[]);

protected:
    /// Application
    const App * fi_app;
    /// Function expressions
    const std::vector<std::string> & expression;
    /// Time derivatives of `expression`
    const std::vector<std::string> expression_t;
    /// Number of parsed function expressions
    unsigned int num_comps;
    /// Function evaluator
    FunctionEvaluator evalr;
    /// Time derivative evaluator
    FunctionEvaluator evalr_t;

public:
    static Parameters parameters();
    static Parameters valid_params_t();
};

} // namespace godzilla
