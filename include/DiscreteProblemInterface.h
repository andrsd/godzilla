#pragma once

#include "petscds.h"
#include "DiscreteProblemInterface.h"

namespace godzilla {

class Logger;
class UnstructuredMesh;
class Problem;
class InitialCondition;
class BoundaryCondition;

/// Interface for discrete problems
///
class DiscreteProblemInterface {
public:
    DiscreteProblemInterface(Problem * problem, const InputParameters & params);
    virtual ~DiscreteProblemInterface();

    /// Get the unstructured mesh
    ///
    /// @return Pointer to the unstructured mesh this problem is using
    virtual const UnstructuredMesh * get_mesh() const;

    /// Get number of fields
    ///
    /// @return The number of fields
    virtual PetscInt get_num_fields() const = 0;

    /// Get list of all field names
    ///
    /// @return List of field names
    virtual std::vector<std::string> get_field_names() const = 0;

    /// Get field name
    ///
    /// @param fid Field ID
    virtual const std::string & get_field_name(PetscInt fid) const = 0;

    /// Get number of field components
    ///
    /// @param fid Field ID
    /// @return Number of components
    virtual PetscInt get_field_num_components(PetscInt fid) const = 0;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    virtual PetscInt get_field_id(const std::string & name) const = 0;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_id(PetscInt fid) const = 0;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_name(const std::string & name) const = 0;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    virtual PetscInt get_field_order(PetscInt fid) const = 0;

    /// Get component name of a field
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @return Component name
    virtual std::string get_field_component_name(PetscInt fid, PetscInt component) const = 0;

    /// Set the name of a component of afield variable
    ///
    /// @param fid Field ID
    /// @param component Component index
    /// @param name Component name
    virtual void
    set_field_component_name(PetscInt fid, PetscInt component, const std::string name) = 0;

    /// Set problem constants
    ///
    /// These constants will be available in the weak form via `constants` parameter.
    /// @param consts Constants to add to the problem
    virtual void set_constants(const std::vector<PetscReal> & consts);

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    virtual void add_initial_condition(InitialCondition * ic);

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object to add
    virtual void add_boundary_condition(BoundaryCondition * bc);

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

    /// Set up constants
    void set_up_constants();

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

    /// Object that manages a discrete system
    PetscDS ds;

    /// List of constants
    std::vector<PetscReal> consts;
};

} // namespace godzilla
