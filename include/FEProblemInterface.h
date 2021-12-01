#pragma once

#include <string>
#include <vector>
#include "petsc.h"
#include "petscfe.h"
#include "petscds.h"

namespace godzilla {

class Logger;
class Grid;
class InitialCondition;
class BoundaryCondition;
class AuxiliaryField;

/// Interface for FE problems
///
/// Any problem using PetscFE should inherit from this for unified API
class FEProblemInterface {
public:
    FEProblemInterface(const InputParameters & params);
    virtual ~FEProblemInterface();

    /// Get field name
    ///
    /// @param fid Field Id
    virtual const std::string & getFieldName(PetscInt fid) const;

    /// Get auxiliary field name
    ///
    /// @param fid Auxiliary field ID
    /// @return Auxiliary field name
    virtual const std::string & getAuxFieldName(PetscInt fid) const;

    /// Get auxiliary field ID
    ///
    /// @param name Auxiliary field name
    /// @param Auxiliary field ID
    virtual PetscInt getAuxFieldID(const std::string & name) const;

    /// Do we have auxiliary field with specified ID
    ///
    /// @param id The ID of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool hasAuxFieldByID(PetscInt fid) const;

    /// Do we have auxiliary field with specified name
    ///
    /// @param name The name of the auxiliary field
    /// @return True if the auxiliary field exists, otherwise False
    virtual bool hasAuxFieldByName(const std::string & name) const;

    /// Adds a volumetric field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void addField(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

    /// Adds a volumetric auxiliary field
    ///
    /// @param id The field ID
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    virtual void addAuxFE(PetscInt id, const std::string & name, PetscInt nc, PetscInt k);

    /// Set problem constants
    ///
    /// These constants will be available in the weak form via `constants` parameter.
    /// @param consts Constants to add to the problem
    virtual void setConstants(std::vector<PetscReal> & consts);

    /// Add initial condition
    ///
    /// @param ic Initial condition object to add
    virtual void addInitialCondition(InitialCondition * ic);

    /// Add essential boundary condition
    ///
    /// @param bc Boundary condition object to add
    virtual void addBoundaryCondition(BoundaryCondition * bc);

    /// Add auxiliary field
    ///
    /// @param aux Auxiliary field object to add
    virtual void addAuxiliaryField(AuxiliaryField * aux);

protected:
    /// Initialize the FE system
    virtual void init(DM dm);

    virtual void create(DM dm);

    virtual void setUpInitialGuess(DM dm, Vec x);

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

    /// Set up finite element objects
    void setUpFEs(DM dm);

    /// Inform PETSc to about all fields in this problem
    void setUpProblem(DM dm);

    /// Set up residual statement for a field variable
    ///
    /// @param fid Field number returned by addField()
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void setResidualBlock(PetscInt fid, PetscFEResidualFunc * f0, PetscFEResidualFunc * f1);

    /// Set up residual statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void setJacobianBlock(PetscInt fid,
                          PetscInt gid,
                          PetscFEJacobianFunc * g0,
                          PetscFEJacobianFunc * g1,
                          PetscFEJacobianFunc * g2,
                          PetscFEJacobianFunc * g3);

    /// Set up boundary conditions
    virtual void setUpBoundaryConditions(DM dm);

    /// Set up auxiliary DM
    virtual void setUpAuxiliaryDM(DM dm);

    /// Set up constants
    void setUpConstants();

    /// Set up field variables
    virtual void onSetFields() = 0;

    /// Setup volumetric weak form terms
    /// FIXME: This needs a better name
    virtual void onSetWeakForm() = 0;

    /// Logger object
    Logger & logger;

    /// Spatial dimension of the discrete problem
    PetscInt dim;

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
