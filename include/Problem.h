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
    virtual DM get_dm() const = 0;
    /// Return solution vector
    virtual Vec get_solution_vector() const = 0;
    /// Get mesh this problem is using
    virtual const Mesh * get_mesh() const;
    /// Get problem spatial dimension
    virtual PetscInt get_dimension() const;

    /// Get simulation time. For steady-state simulations, time is always 0
    ///
    /// @return Simulation time
    const PetscReal & get_time() const;

    /// Get time step number
    ///
    /// @return Time step number
    const PetscInt & get_step_num() const;

    /// Get list of functions
    ///
    /// @return List of functions
    const std::vector<Function *> & get_functions() const;

    /// Add a function object
    ///
    /// @param fn Function object to add
    virtual void add_function(Function * fn);

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void add_output(Output * output);

    /// Add a postprocessor object
    ///
    /// @param pp Postprocessor object to add
    virtual void add_postprocessor(Postprocessor * pp);

    /// Get postprocessor by name
    ///
    /// @param name The name of the postprocessor
    /// @return Pointer to the postprocessor with name 'name' if it exists, otherwise `nullptr`
    virtual Postprocessor * get_postprocessor(const std::string & name) const;

    /// Get postprocessor names
    ///
    /// @return List of postprocessor names
    virtual const std::vector<std::string> & get_postprocessor_names() const;

    /// Compute all postprocessors
    virtual void compute_postprocessors();

    /// Output
    ///
    /// @param mask Bit mask for an output event, see `Output` for valid options.
    virtual void output(unsigned int mask);

protected:
    /// Called before solving starts
    virtual void on_initial();
    /// Called after solve has successfully finished
    virtual void on_final();

    /// Mesh
    const Mesh * mesh;

    /// List of functions
    std::vector<Function *> functions;

    /// List of output objects
    std::vector<Output *> outputs;

    /// Default output execute mask
    unsigned int default_output_on;

    /// List of postprocessor objects
    std::map<std::string, Postprocessor *> pps;

    /// List of postprocesso names
    std::vector<std::string> pps_names;

    /// Simulation time
    PetscReal time;

    /// Time step number
    PetscInt step_num;

public:
    static InputParameters valid_params();

    friend class TransientProblemInterface;
};

} // namespace godzilla
