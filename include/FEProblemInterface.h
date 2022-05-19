#pragma once

#include <string>
#include <vector>
#include "petsc.h"
#include "petscfe.h"
#include "petscds.h"

namespace godzilla {

class Logger;
class UnstructuredMesh;
class Problem;
class InitialCondition;
class BoundaryCondition;
class AuxiliaryField;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface {
public:
    FEProblemInterface(Problem * problem, const InputParameters & params);
    virtual ~FEProblemInterface();

    /// Get list of all field names
    ///
    /// @return List of field names
    virtual std::vector<std::string> get_field_names() const;

    /// Get field name
    ///
    /// @param fid Field ID
    virtual const std::string & get_field_name(PetscInt fid) const;

    /// Get field order
    ///
    /// @param fid Field ID
    /// @return Field order
    virtual PetscInt get_field_order(PetscInt fid) const;

    /// Get field number of components
    ///
    /// @param fid Field ID
    /// @return Number of components
    virtual PetscInt get_field_num_components(PetscInt fid) const;

    /// Get field ID
    ///
    /// @param name Field name
    /// @param Field ID
    virtual PetscInt get_field_id(const std::string & name) const;

    /// Do we have field with specified ID
    ///
    /// @param fid The ID of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_id(PetscInt fid) const;

    /// Do we have field with specified name
    ///
    /// @param name The name of the field
    /// @return True if the field exists, otherwise False
    virtual bool has_field_by_name(const std::string & name) const;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    virtual const std::string & get_aux_field_name(PetscInt fid) const;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @param Auxiliary field ID
    virtual PetscInt get_aux_field_id(const std::string & name) const;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param fid The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_id(PetscInt fid) const;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool has_aux_field_by_name(const std::string & name) const;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

    /// Adds a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void add_aux_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

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

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    virtual void add_auxiliary_field(AuxiliaryField * aux);

    /// Get the simulation time (the time is pulled from the linked Problem class)
    ///
    /// @return The simulation time
    virtual const PetscReal & get_time() const;

protected:
    struct FieldInfo;

    /// Initialize the FE system
    virtual void init();

    virtual void create();

    virtual void set_up_initial_guess(DM dm, Vec x);

    typedef void PetscFEResidualFunc(PetscInt dim,
                                     PetscInt Nf,
                                     PetscInt NfAux,
                                     const PetscInt uOff[],
                                     const PetscInt uOff_x[],
                                     const PetscScalar u[],
                                     const PetscScalar u_t[],
                                     const PetscScalar u_x[],
                                     const PetscInt aOff[],
                                     const PetscInt aOff_x[],
                                     const PetscScalar a[],
                                     const PetscScalar a_t[],
                                     const PetscScalar a_x[],
                                     PetscReal t,
                                     const PetscReal x[],
                                     PetscInt numConstants,
                                     const PetscScalar constants[],
                                     PetscScalar f0[]);

    typedef void PetscFEJacobianFunc(PetscInt dim,
                                     PetscInt Nf,
                                     PetscInt NfAux,
                                     const PetscInt uOff[],
                                     const PetscInt uOff_x[],
                                     const PetscScalar u[],
                                     const PetscScalar u_t[],
                                     const PetscScalar u_x[],
                                     const PetscInt aOff[],
                                     const PetscInt aOff_x[],
                                     const PetscScalar a[],
                                     const PetscScalar a_t[],
                                     const PetscScalar a_x[],
                                     PetscReal t,
                                     PetscReal u_tShift,
                                     const PetscReal x[],
                                     PetscInt numConstants,
                                     const PetscScalar constants[],
                                     PetscScalar g3[]);

    /// Create FE object from FieldInfo
    ///
    /// @param fi Field description
    void create_fe(FieldInfo & fi);

    /// Set up finite element objects
    void set_up_fes();

    /// Inform PETSc to about all fields in this problem
    void set_up_problem();

    /// Set up residual statement for a field variable
    ///
    /// @param fid Field ID
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void set_residual_block(PetscInt fid, PetscFEResidualFunc * f0, PetscFEResidualFunc * f1);

    /// Set up residual statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void set_jacobian_block(PetscInt fid,
                            PetscInt gid,
                            PetscFEJacobianFunc * g0,
                            PetscFEJacobianFunc * g1,
                            PetscFEJacobianFunc * g2,
                            PetscFEJacobianFunc * g3);

    /// Set up boundary conditions
    virtual void set_up_boundary_conditions();

    /// Set up auxiliary DM
    virtual void set_up_auxiliary_dm(DM dm);

    /// Set up constants
    void set_up_constants();

    /// Set up field variables
    virtual void on_set_fields() = 0;

    /// Setup volumetric weak form terms
    /// FIXME: This needs a better name
    virtual void on_set_weak_form() = 0;

    /// Problem this interface is part of
    Problem * problem;

    /// Unstructured mesh
    const UnstructuredMesh * unstr_mesh;

    /// Logger object
    Logger * logger;

    /// Quadrature order
    PetscInt qorder;

    /// Field information
    struct FieldInfo {
        /// The name of the field
        std::string name;

        /// Field number
        PetscInt id;

        /// FE object
        PetscFE fe;

        /// Mesh support
        DMLabel block;

        /// The number of components
        PetscInt nc;

        /// The degree k of the space
        PetscInt k;
    };

    /// Fields in the problem
    std::map<PetscInt, FieldInfo> fields;

    /// Map from field name to field ID
    std::map<std::string, PetscInt> fields_by_name;

    /// Initial condition information
    struct ICInfo {
        /// Initial condition object
        InitialCondition * ic;
    };

    /// Initial conditions in the problem
    std::map<PetscInt, ICInfo> ics;

    /// List of boundary conditions
    std::vector<BoundaryCondition *> bcs;

    /// Auxiliary fields in the problem
    std::map<PetscInt, FieldInfo> aux_fields;

    /// Map from auxiliary field name to auxiliary field ID
    std::map<std::string, PetscInt> aux_fields_by_name;

    /// List of auxiliary field objects
    std::vector<AuxiliaryField *> auxs;

    /// Object that manages a discrete system
    PetscDS ds;

    /// List of constants
    std::vector<PetscReal> consts;
};

} // namespace godzilla
