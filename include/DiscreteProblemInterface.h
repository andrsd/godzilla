#pragma once

#include "petscds.h"
#include "Types.h"
#include "Vector.h"
#include "DiscreteProblemInterface.h"

namespace godzilla {

class Parameters;
class Logger;
class UnstructuredMesh;
class Problem;
class WeakForm;
class InitialCondition;
class BoundaryCondition;

/// Interface for discrete problems
///
class DiscreteProblemInterface {
public:
    DiscreteProblemInterface(Problem * problem, const Parameters & params);
    virtual ~DiscreteProblemInterface() = default;

    /// Get the unstructured mesh
    ///
    /// @return Pointer to the unstructured mesh this problem is using
    virtual const UnstructuredMesh * get_mesh() const;

    /// Get number of fields
    ///
    /// @return The number of fields
    virtual Int get_num_fields() const = 0;

    /// Get list of all field names
    ///
    /// @return List of field names
    virtual std::vector<std::string> get_field_names() const = 0;

    /// Get field name
    ///
    /// @param fid Field ID
    virtual const std::string & get_field_name(Int fid) const = 0;

    /// Get number of field components
    ///
    /// @param fid Field ID
    /// @return Number of components
    virtual Int get_field_num_components(Int fid) const = 0;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    virtual Int get_field_id(const std::string & name) const = 0;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_id(Int fid) const = 0;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_name(const std::string & name) const = 0;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    virtual Int get_field_order(Int fid) const = 0;

    /// Get component name of a field
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_field_component_name(Int fid, Int component) const = 0;

    /// Set the name of a component of afield variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void set_field_component_name(Int fid, Int component, const std::string & name) = 0;

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    virtual void add_initial_condition(InitialCondition * ic);

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object to add
    virtual void add_boundary_condition(BoundaryCondition * bc);

    /// Return the offset into an array or local Vec for the dof associated with the given point
    ///
    /// @param point Point
    /// @param fid Field ID
    /// @return The offset
    virtual Int get_field_dof(Int point, Int fid) const = 0;

    /// Gets a local vector with the coordinates associated with this problem's mesh
    ///
    /// @return coordinate vector
    Vector get_coordinates_local() const;

    /// Get local solution vector
    ///
    /// @return Local solution vector
    virtual const Vector & get_solution_vector_local() const = 0;

    /// Get weak form associated with this problem
    ///
    /// @return The weak form associated with this problem
    virtual WeakForm * get_weak_form() const = 0;

    // NOTE: these may be pushed down into child classes if needed
    virtual void add_boundary_essential(const std::string & name,
                                        DMLabel label,
                                        const std::vector<Int> & ids,
                                        Int field,
                                        const std::vector<Int> & components,
                                        PetscFunc * fn,
                                        PetscFunc * fn_t,
                                        void * context) const = 0;
    virtual void add_boundary_natural(const std::string & name,
                                      DMLabel label,
                                      const std::vector<Int> & ids,
                                      Int field,
                                      const std::vector<Int> & components,
                                      void * context) const = 0;
    virtual void add_boundary_natural_riemann(const std::string & name,
                                              DMLabel label,
                                              const std::vector<Int> & ids,
                                              Int field,
                                              const std::vector<Int> & components,
                                              PetscNaturalRiemannBCFunc * fn,
                                              PetscNaturalRiemannBCFunc * fn_t,
                                              void * context) const = 0;

protected:
    virtual void init();
    virtual void create();
    /// Set up discrete system
    virtual void set_up_ds() = 0;

    /// Set up field variables
    virtual void set_up_fields() = 0;

    void set_zero_initial_guess();

    void set_initial_guess_from_ics();

    virtual void set_up_initial_guess();

    /// Set up initial conditions
    virtual void set_up_initial_conditions();

    /// Set up boundary conditions
    virtual void set_up_boundary_conditions();

    /// Build local solution vector
    ///
    /// @param sln Global solution vector
    void build_local_solution_vector(const Vector & sln) const;

    /// Problem this interface is part of
    Problem * problem;

    /// Unstructured mesh
    const UnstructuredMesh * unstr_mesh;

    /// Logger object
    Logger * logger;

    /// Initial conditions in the problem
    std::vector<InitialCondition *> ics;

    /// List of boundary conditions
    std::vector<BoundaryCondition *> bcs;
};

} // namespace godzilla
