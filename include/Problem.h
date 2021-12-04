#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class Grid;
class Output;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    Problem(const InputParameters & parameters);

    virtual void check() override;

    /// Build the problem to solve
    virtual void create() override;
    /// Run the problem
    virtual void run() = 0;
    /// Solve the problem
    virtual void solve() = 0;
    /// true if solve converged, otherwise false
    virtual bool converged() = 0;
    /// provide DM for the underlying KSP object
    virtual DM getDM() const = 0;
    /// Return solution vector
    virtual Vec getSolutionVector() const = 0;

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void addOutput(Output * output);

protected:
    /// Grid
    Grid & grid;
    /// List of output objects
    std::vector<Output *> outputs;

public:
    static InputParameters validParams();
};

} // namespace godzilla
