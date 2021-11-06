#pragma once

#include "problems/GPetscNonlinearProblem.h"
#include "petscsnes.h"
#include "petscfe.h"
#include "petscds.h"

namespace godzilla {

/// PETSc non-linear problem that arises from a finite element discretization
/// using the PetscFE system
///
class GPetscFENonlinearProblem : public GPetscNonlinearProblem
{
public:
    GPetscFENonlinearProblem(const InputParameters & parameters);
    virtual ~GPetscFENonlinearProblem();

    virtual void create() override;

protected:
    virtual void init() override;
    virtual void setupCallbacks() override;
    virtual void setupInitialGuess() override;
    virtual PetscErrorCode computeResidualCallback(Vec x, Vec f) override;
    virtual PetscErrorCode computeJacobianCallback(Vec x, Mat J, Mat Jp) override;

    typedef
    PetscErrorCode PetscFunc(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar u[], void *ctx);

    typedef
    void PetscFEResidualFunc(PetscInt dim, PetscInt Nf, PetscInt NfAux,
        const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
        const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
        PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f0[]);

    typedef
    void PetscFEJacobianFunc(PetscInt dim, PetscInt Nf, PetscInt NfAux,
        const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
        const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
        PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar g3[]);

    typedef
    void PetscFieldFunc(PetscInt dim, PetscInt Nf, PetscInt NfAux,
        const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
        const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
        PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[], PetscScalar f[]);

    /// Adds a volumetric field
    ///
    /// @param name The name of the field
    /// @param nc The number of components
    /// @param k The degree k of the space
    PetscInt addField(const std::string & name, PetscInt nc, PetscInt k);

    /// Setup residual statement for a field variable
    ///
    /// @param fid Field number returned by addField()
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void setResidualBlock(PetscInt fid, PetscFEResidualFunc *f0, PetscFEResidualFunc *f1);

    /// Setup residual statement for a field variable
    ///
    /// @param fid Test field number
    /// @param gid Field number
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void setJacobianBlock(PetscInt fid, PetscInt gid, PetscFEJacobianFunc *g0, PetscFEJacobianFunc *g1, PetscFEJacobianFunc *g2, PetscFEJacobianFunc *g3);

    /// Setup initial conditions
    ///
    /// @param fid Field number
    /// @param ic Initial condition function
    void setInitialCondition(PetscFunc *ic);
    void setInitialCondition(PetscInt fid, PetscFieldFunc *ic);

    /// Setup boudary conditions
    virtual void setupBoundaryConditions();

    /// Seup field varaibles
    virtual void onSetFields() = 0;
    /// Setup volumetric weak form terms
    /// FIXME: This needs a better name
    virtual void onSetWeakForm() = 0;
    /// Setup boudary conditions
    virtual void onSetInitialConditions() = 0;


    /// Spatial dimension of the discrete problem
    PetscInt dim;

    /// Field ID counter
    PetscInt field_id;
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
    std::map<PetscInt, FieldInfo> fields;
    /// Field initial conditions
    std::map<PetscInt, PetscFieldFunc *> field_ics;
    /// Object that manages a discrete system
    PetscDS ds;

public:
    static InputParameters validParams();
};

}
