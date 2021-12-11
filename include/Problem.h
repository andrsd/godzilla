#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"
#include "petscpartitioner.h"

namespace godzilla {

class Grid;
class Postprocessor;
class Output;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    Problem(const InputParameters & parameters);
    virtual ~Problem();

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

    /// Add a postprocessor object
    ///
    /// @param pp Postprocessor object to add
    virtual void addPostprocessor(Postprocessor * pp);

protected:
    /// Compute all postprocessors
    virtual void computePostprocessors();

    /// Grid
    Grid & grid;

    /// List of output objects
    std::vector<Output *> outputs;

    /// List of postprocessor objects
    std::vector<Postprocessor *> pps;

public:
    static InputParameters validParams();
};

} // namespace godzilla
