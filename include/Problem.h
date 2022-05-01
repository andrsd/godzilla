#pragma once

#include "GodzillaConfig.h"
#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"
#include "petscpartitioner.h"

namespace godzilla {

class Output;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    Problem(const InputParameters & parameters);
    virtual ~Problem();

    /// provide DM for the underlying PETSc object
    virtual DM get_dm() const = 0;

    virtual void check() override;

    /// Build the problem to solve
    virtual void create() override;
    /// Run the problem
    virtual void run() = 0;
    /// Solve the problem
    virtual void solve() = 0;
    /// true if solve converged, otherwise false
    virtual bool converged() = 0;
    /// Return solution vector
    virtual Vec get_solution_vector() const = 0;

    /// Get simulation time. For steady-state simulations, time is always 0
    ///
    /// @return Simulation time
    virtual const Real & get_time() const;

    /// Add an output object
    ///
    /// @param output Output object to add
    virtual void add_output(Output * output);

    /// Produce output
    virtual void output();

protected:
    /// Simulation time
    Real time;

    /// List of output objects
    std::vector<Output *> outputs;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
