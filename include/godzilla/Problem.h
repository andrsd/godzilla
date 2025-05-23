// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Vector.h"
#include "godzilla/Matrix.h"
#include "godzilla/IndexSet.h"
#include "godzilla/Partitioner.h"
#include "godzilla/Label.h"
#include "godzilla/StarForest.h"

namespace godzilla {

class MeshObject;
class Mesh;
class Function;
class Postprocessor;
class Output;
class FileOutput;
class Section;
template <typename T>
class Array1D;

/// Problem
///
class Problem : public Object, public PrintInterface {
public:
    struct FieldDecomposition {
        /// Field names
        std::vector<std::string> field_name;
        /// Global indices for each field
        std::vector<IndexSet> is;

        FieldDecomposition(Int n = 0);

        Int get_num_fields() const;

        void destroy();
    };

    explicit Problem(const Parameters & parameters);
    ~Problem() override;

    /// Build the problem to solve
    void create() override;

    /// Run the problem
    virtual void run() = 0;

    /// Provide DM for this problem
    DM get_dm() const;

    /// Return solution vector
    const Vector & get_solution_vector() const;
    Vector & get_solution_vector();

    /// Get problem spatial dimension
    Int get_dimension() const;

    /// Get simulation time. For steady-state simulations, time is always 0
    ///
    /// @return Simulation time
    virtual Real get_time() const;

    /// Get time step number
    ///
    /// @return Time step number
    virtual Int get_step_num() const;

    /// Get list of functions
    ///
    /// @return List of functions
    const std::vector<Function *> & get_functions() const;

    /// Add a function object
    ///
    /// @param fn Function object to add
    void add_function(Function * fn);

    /// Add and output object
    ///
    /// @param output Output object to add
    void add_output(Output * output);

    /// Add a postprocessor object
    ///
    /// @param pp Postprocessor object to add
    void add_postprocessor(Postprocessor * pp);

    /// Get postprocessor by name
    ///
    /// @param name The name of the postprocessor
    /// @return Pointer to the postprocessor with name 'name' if it exists, otherwise `nullptr`
    Postprocessor * get_postprocessor(const std::string & name) const;

    /// Get postprocessor names
    ///
    /// @return List of postprocessor names
    const std::vector<std::string> & get_postprocessor_names() const;

    /// Compute all postprocessors
    void compute_postprocessors();

    /// Output
    ///
    /// @param mask Bit mask for an output event, see `Output` for valid options.
    void output(ExecuteOnFlag flag);

    /// Gets the type of vector created with `create_local_vector` and `create_global_vector`
    ///
    /// @return The vector type
    std::string get_vector_type() const;

    /// Sets the type of vector to be created with `create_local_vector` and `create_global_vector`
    ///
    /// @param type The vector type, for example `VECSTANDARD`, `VECCUDA`, or `VECVIENNACL`
    void set_vector_type(const std::string & type);

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

    /// Gets the type of matrix that would be created with `create_matrix`
    ///
    /// @return The matrix type
    std::string get_matrix_type() const;

    /// Sets the type of matrix created with `create_matrix`
    ///
    /// @param type the matrix type, for example `MATMPIAIJ`
    void set_matrix_type(const std::string & type);

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

    /// Set default execute on flags
    void set_default_output_on(ExecuteOn flags);

    /// Create field decomposition
    FieldDecomposition create_field_decomposition();

    /// Get the partitioner associated with this problem
    ///
    /// @return Partitioner
    const Partitioner & get_partitioner();

    /// Set partitioner type
    ///
    /// @param type Type of the partitioner
    void set_partitioner_type(const std::string & type);

    /// Set partitioner overlap
    ///
    /// @param type Type of the partitioner
    void set_partition_overlap(Int overlap);

    /// Get partition overlap
    ///
    /// @return Partition overlap
    Int get_partition_overlap();

    /// Get the number of auxiliary vectors
    ///
    /// @return The number of auxiliary data vectors
    Int get_num_auxiliary_vec() const;

    /// Get the auxiliary vector for region specified by the given label, value, and equation part
    ///
    /// @param label The `Label`
    /// @param value The label value indicating the region
    /// @param part The equation part, or 0 if unused
    /// @return The `Vector` holding auxiliary field data
    Vector get_auxiliary_vec(const Label & label, Int value, Int part = 0) const;

    /// Set an auxiliary vector for region specified by the given label, value, and equation part
    ///
    /// @param label The `Label`
    /// @param value The label value indicating the region
    /// @param part The equation part, or 0 if unused
    /// @param vec The `Vector` holding auxiliary field data
    void set_auxiliary_vec(const Label & label, Int value, Int part, const Vector & vec);

    /// Destroys the auxiliary vector information and creates a new empty one
    void clear_auxiliary_vec();

    /// Returns an IndexSet containing a Section that encapsulates a subproblem defined by
    /// a subset of the fields in a Section in the problem.
    ///
    /// @param fields The field numbers of the selected fields
    /// @return The global indices for the subproblem
    IndexSet create_section_subis(const std::vector<Int> & fields) const;

    /// Returns an IndexSet containing a Section that encapsulates a subproblem defined by
    /// a subset of the fields and components in a Section in the problem.
    ///
    /// @param fields The field numbers of the selected fields
    /// @param n_comps The number of components from each field to incorporate into the subproblem
    /// @param comps The component numbers of the selected fields
    /// @return The global indices for the subproblem
    IndexSet create_section_subis(const std::vector<Int> & fields,
                                  const std::vector<Int> & n_comps,
                                  const std::vector<Int> & comps) const;

    /// Set output monitor
    ///
    /// @tparam T C++ class type
    /// @param instance Instance of the class
    /// @param monitor Member function in class T
    template <class T>
    void
    set_output_monitor(T * instance, void (T::*method)(const std::string &) const)
    {
        this->output_monitor_delegate.bind(instance, method);
    }

    /// Set the flags for determining variable influence
    ///
    /// @param field Field number
    /// @param use_cone Flag for variable influence starting with the cone operation
    /// @param use_closure Flag for variable influence using transitive closure
    void set_adjacency(Int field, bool use_cone, bool use_closure);

    /// Updates global vector from local vector
    ///
    /// @param l Local vector
    /// @param mode Insert mode
    /// @param g Global vector
    void local_to_global(const Vector & l, InsertMode mode, Vector & g) const;

    /// Updates local vector from global vector
    ///
    /// @param g Global vector
    /// @param mode Insert mode
    /// @param l Local vector
    void global_to_local(const Vector & g, InsertMode mode, Vector & l) const;

protected:
    /// Set vector/matrix types
    virtual void set_up_types();

    /// Allocate objects
    virtual void allocate_objects();

    /// Called before solving starts
    virtual void on_initial();

    /// Called after solve has successfully finished
    virtual void on_final();

    /// Set solution vector
    void set_solution_vector(const Vector & x);

private:
    /// Output monitor
    ///
    /// @param file_name Name of the output file
    void output_monitor(const std::string & file_name) const;

    /// Mesh
    MeshObject * mesh;

    /// Mesh partitioner
    Partitioner partitioner;

    /// Partition overlap for mesh partitioning
    Int partition_overlap;

    /// The solution vector
    Vector x;

    /// List of functions
    std::vector<Function *> functions;

    /// List of output objects
    std::vector<Output *> outputs;
    std::vector<FileOutput *> file_outputs;

    /// Default output execute mask
    ExecuteOn default_output_on;

    /// List of postprocessor objects
    std::map<std::string, Postprocessor *> pps;

    /// List of postprocessor names
    std::vector<std::string> pps_names;

    /// Output monitor
    Delegate<void(const std::string &)> output_monitor_delegate;

public:
    static Parameters parameters();
};

/// Move values from a local vector to a global one
///
/// @param dm Data manager
/// @param l Local vector
/// @param mode Operation
/// @param g Global vector
void local_to_global(DM dm, const Vector & l, InsertMode mode, Vector & g);

/// Move values from a global vector to a local one
///
/// @param dm Data manager
/// @param g Global vector
/// @param mode Operation
/// @param l Local vector
void global_to_local(DM dm, const Vector & g, InsertMode mode, Vector & l);

/// Get a vector that may be used with the DM local routines. This vector has spaces for the
/// ghost values.
///
/// @return Local vector
Vector get_local_vector(DM dm);

void restore_local_vector(DM dm, const Vector & g);

/// Get global vector
Vector get_global_vector(DM dm);

/// Restore global vector
void restore_global_vector(DM dm, const Vector & g);

/// Create local vector
///
/// @param dm Data manager
/// @return New local vector
Vector create_local_vector(DM dm);

/// Creates a global vector from a DM object. A global vector is a parallel vector that has no
/// duplicate values shared between MPI ranks, that is it has no ghost locations.
///
/// @param dm Data manager
/// @return New global vector
Vector create_global_vector(DM dm);

/// Get the Section encoding the local data layout for the DM
///
/// @param dm Data manager
Section get_local_section(DM dm);

/// Get the Section encoding the global data layout for the DM
///
/// @param dm Data manager
Section get_global_section(DM dm);

/// Get the PetscSF encoding the parallel dof overlap for the DM. If it has not been set, it is
/// created from the default PetscSection layouts in the DM.
///
/// @param dm Data manager
StarForest get_section_star_forest(DM dm);

/// Equivalent of `create_local_vector` but for `Array1D`
///
/// @param dm Data manager
template <typename T>
Array1D<T>
create_local_array1d(DM dm)
{
    CALL_STACK_MSG();
    auto section = get_local_section(dm);
    auto size = section.get_storage_size();
    return Array1D<T>(size);
}

/// Equivalent of `create_global_vector` but for `Array1D`
///
/// @param dm Data manager
template <typename T>
Array1D<T>
create_global_array1d(DM dm)
{
    CALL_STACK_MSG();
    auto section = get_global_section(dm);
    auto size = section.get_constrained_storage_size();
    return Array1D<T>(size);
}

///

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
void
local_to_global(DM dm, const Array1D<T> & l, InsertMode mode, Array1D<T> & g)
{
    CALL_STACK_MSG();
    auto sf = get_section_star_forest(dm);
    const T * l_array = l.get_data();
    T * g_array = g.get_data();
    switch (mode) {
    case ADD_VALUES:
        sf.reduce_begin(l_array, g_array, mpi::op::sum<T>());
        sf.reduce_end(l_array, g_array, mpi::op::sum<T>());
        break;
    case MAX_VALUES:
        sf.reduce_begin(l_array, g_array, mpi::op::max<T>());
        sf.reduce_end(l_array, g_array, mpi::op::max<T>());
        break;
    case MIN_VALUES:
        sf.reduce_begin(l_array, g_array, mpi::op::min<T>());
        sf.reduce_end(l_array, g_array, mpi::op::min<T>());
        break;
    default:
        throw Exception("Unknown mode");
    }
}

template <typename T, std::enable_if_t<!std::is_arithmetic<T>::value, int> = 0>
void
local_to_global(DM dm, const Array1D<T> & l, InsertMode mode, Array1D<T> & g)
{
    CALL_STACK_MSG();
    auto sf = get_section_star_forest(dm);
    const T * l_array = l.get_data();
    T * g_array = g.get_data();
    switch (mode) {
    case ADD_VALUES:
        sf.reduce_begin(l_array, g_array, mpi::op::sum<T>());
        sf.reduce_end(l_array, g_array, mpi::op::sum<T>());
        break;
    default:
        throw Exception("Unknown mode");
    }
}

template <typename T>
void
global_to_local(DM dm, const Array1D<T> & g, InsertMode mode, Array1D<T> & l)
{
    CALL_STACK_MSG();
    assert(mode != ADD_VALUES);
    auto sf = get_section_star_forest(dm);
    sf.broadcast_begin(g, l, mpi::op::replace<T>());
    sf.broadcast_end(g, l, mpi::op::replace<T>());
}

/// Creates a DM object with the same topology as the original.
///
/// @param dm Data manager
/// @return Cloned data manager
DM clone(DM dm);

} // namespace godzilla
