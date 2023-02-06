#pragma once

#include "Function.h"
#include "FunctionEvaluator.h"
#include "muParser/muParser.h"
#include "Types.h"

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
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param nc The number of components
    /// @param u The computed values
    virtual void evaluate(Int dim, PetscReal time, const PetscReal x[], Int nc, PetscScalar u[]);

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

protected:
    /// Text representation of the function to evaluate (one per component)
    const std::vector<std::string> & function;
    /// User defined constants
    const std::map<std::string, PetscReal> & constants;
    /// Function evaluator
    FunctionEvaluator evalr;

public:
    static Parameters parameters();
};

} // namespace godzilla
