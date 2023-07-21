#pragma once

#include "Types.h"
#include "Object.h"
#include "PrintInterface.h"
#include "Vector.h"
#include "Matrix.h"
#include "petscdm.h"
#include "petscpartitioner.h"

namespace godzilla {

class Mesh;
class Function;
class Postprocessor;
class Output;
class Section;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    explicit Problem(const Parameters & parameters);

    void check() override;

    /// Build the problem to solve
    void create() override;
    /// Run the problem
    virtual void run() = 0;
    /// Provide DM for this problem
    virtual DM get_dm() const;
    /// Return solution vector
    virtual const Vector & get_solution_vector() const = 0;
    /// Get mesh this problem is using
    virtual Mesh * get_mesh() const;
    /// Get problem spatial dimension
    virtual Int get_dimension() const;

    /// Get simulation time. For steady-state simulations, time is always 0
    ///
    /// @return Simulation time
    const Real & get_time() const;

    /// Get time step number
    ///
    /// @return Time step number
    const Int & get_step_num() const;

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

    /// Creates a local vector from a DM object
    ///
    /// @return New local vector
    Vector create_local_vector() const;

    /// Get a vector that may be used with the DM local routines. This vector has spaces for the
    /// ghost values.
    ///
    /// @return Local vector
    Vector get_local_vector() const;

    void restore_local_vector(const Vector & vec) const;

    /// Creates a global vector from a DM object. A global vector is a parallel vector that has no
    /// duplicate values shared between MPI ranks, that is it has no ghost locations.
    ///
    /// @return New global vector
    Vector create_global_vector() const;

    /// Get a vector that may be used with the DM global routines
    ///
    /// @return Global vector
    Vector get_global_vector() const;

    void restore_global_vector(const Vector & vec) const;

    /// Get an empty matrix for a `DM`
    Matrix create_matrix() const;

    /// Get the Section encoding the local data layout for the DM
    Section get_local_section() const;

    /// Set the `Section` encoding the local data layout for the `DM`.
    void set_local_section(const Section & section) const;

    /// Get the Section encoding the global data layout for the DM
    Section get_global_section() const;

    /// Set the `Section` encoding the global data layout for the `DM`.
    void set_global_section(const Section & section) const;

protected:
    /// Called before solving starts
    virtual void on_initial();
    /// Called after solve has successfully finished
    virtual void on_final();

    /// Mesh
    Mesh * mesh;

    /// List of functions
    std::vector<Function *> functions;

    /// List of output objects
    std::vector<Output *> outputs;

    /// Default output execute mask
    unsigned int default_output_on;

    /// List of postprocessor objects
    std::map<std::string, Postprocessor *> pps;

    /// List of
    /// postprocessor names
    std::vector<std::string> pps_names;

    /// Simulation time
    Real time;

    /// Time step number
    Int step_num;

public:
    static Parameters parameters();

    friend class TransientProblemInterface;
};

} // namespace godzilla
