#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"
#include "petscpartitioner.h"

namespace godzilla {

class Mesh;
class Function;
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
    /// Get problem spatial dimension
    virtual PetscInt getDimension() const;

    /// Get simulation time. For steady-state simulations, time is always 0
    ///
    /// @return Simulation time
    virtual const PetscReal & getTime() const;

    /// Get list of functions
    ///
    /// @return List of functions
    const std::vector<Function *> & getFunctions() const;

    /// Add a function object
    ///
    /// @param fn Function object to add
    virtual void addFunction(Function * fn);

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void addOutput(Output * output);

    /// Add a postprocessor object
    ///
    /// @param pp Postprocessor object to add
    virtual void addPostprocessor(Postprocessor * pp);

    /// Get postprocessor by name
    ///
    /// @param name The name of the postprocessor
    /// @return Pointer to the postprocessor with name 'name' if it exists, otherwise `nullptr`
    virtual Postprocessor * getPostprocessor(const std::string & name) const;

protected:
    /// Compute all postprocessors
    virtual void computePostprocessors();

    /// Mesh
    const Mesh * mesh;

    /// List of functions
    std::vector<Function *> functions;

    /// List of output objects
    std::vector<Output *> outputs;

    /// List of postprocessor objects
    std::map<std::string, Postprocessor *> pps;

    /// Simulation time
    PetscReal time;

public:
    static InputParameters validParams();
};

} // namespace godzilla
