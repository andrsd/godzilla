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
    /// @param dim The spatial dimension
    /// @param time The time at which to sample
    /// @param x The coordinates
    /// @param nc The number of components
    /// @param u The computed values
    virtual void evaluate(Int dim, Real time, const Real x[], Int nc, Scalar u[]);

    /// Get pointer to the C function that will be passed into PETSc API
    virtual PetscFunc * get_function();

    /// Get the pointer to the context that will be passed into PETSc API
    virtual void * get_context();

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
