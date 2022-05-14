#pragma once

#include "BoundaryCondition.h"

namespace godzilla {

/// Base class for natural boundary conditions
class NaturalBC : public BoundaryCondition {
public:
    NaturalBC(const InputParameters & params);

    virtual DMBoundaryConditionType get_bc_type() const override;

protected:
    typedef void PetscFEBndResidualFunc(PetscInt dim,
                                        PetscInt nf,
                                        PetscInt nf_aux,
                                        const PetscInt u_off[],
                                        const PetscInt u_off_x[],
                                        const PetscScalar u[],
                                        const PetscScalar u_t[],
                                        const PetscScalar u_x[],
                                        const PetscInt a_off[],
                                        const PetscInt a_off_x[],
                                        const PetscScalar a[],
                                        const PetscScalar a_t[],
                                        const PetscScalar a_x[],
                                        PetscReal t,
                                        const PetscReal x[],
                                        const PetscReal n[],
                                        PetscInt num_constants,
                                        const PetscScalar constants[],
                                        PetscScalar f[]);

    typedef void PetscFEBndJacobianFunc(PetscInt dim,
                                        PetscInt nf,
                                        PetscInt nf_aux,
                                        const PetscInt u_off[],
                                        const PetscInt u_off_x[],
                                        const PetscScalar u[],
                                        const PetscScalar u_t[],
                                        const PetscScalar u_x[],
                                        const PetscInt a_off[],
                                        const PetscInt a_off_x[],
                                        const PetscScalar a[],
                                        const PetscScalar a_t[],
                                        const PetscScalar a_x[],
                                        PetscReal t,
                                        PetscReal u_t_shift,
                                        const PetscReal x[],
                                        const PetscReal n[],
                                        PetscInt num_constants,
                                        const PetscScalar constants[],
                                        PetscScalar g[]);

    /// Set residual statement for the boundary integral
    ///
    /// @param f0 Integrand for the test function term
    /// @param f1 Integrand for the test function gradient term
    void set_residual_block(PetscFEBndResidualFunc * f0, PetscFEBndResidualFunc * f1);

    /// Set Jacobian statement for the boundary integral
    ///
    /// @param gid Field ID
    /// @param g0 Integrand for the test and basis function term
    /// @param g1 Integrand for the test function and basis function gradient term
    /// @param g2 Integrand for the test function gradient and basis function term
    /// @param g3 Integrand for the test function gradient and basis function gradient term
    void set_jacobian_block(PetscInt gid,
                            PetscFEBndJacobianFunc * g0,
                            PetscFEBndJacobianFunc * g1,
                            PetscFEBndJacobianFunc * g2,
                            PetscFEBndJacobianFunc * g3);

    virtual void set_up_callback() override;

    /// Set up the weak form for the boundary integral of this boundary condition
    virtual void on_set_weak_form() = 0;

    /// WeakForm object
    PetscWeakForm wf;

    /// Boundary number
    PetscInt bd;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
