#include "Godzilla.h"
#include "FunctionInterface.h"
#include "NSIncompressibleProblem.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "CallStack.h"
#include <cassert>
#include "petscsys.h"

using namespace godzilla;

REGISTER_OBJECT(NSIncompressibleProblem);

namespace {

class ResidualVeloc0 : public ResidualFunc {
public:
    explicit ResidualVeloc0(NSIncompressibleProblem * prob) :
        ResidualFunc(prob),
        n_comp(get_spatial_dimension()),
        dim(get_spatial_dimension()),
        vel(get_field_value("velocity")),
        vel_t(get_field_dot("velocity")),
        vel_x(get_field_gradient("velocity")),
        ffn(get_field_value("ffn"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        for (PetscInt c = 0; c < this->n_comp; ++c) {
            f[c] = this->vel_t(c);

            for (PetscInt d = 0; d < this->dim; ++d)
                f[c] += this->vel(d) * this->vel_x(d, c);

            f[c] -= this->ffn(c);
        }
    }

protected:
    const PetscInt & n_comp;
    const PetscInt & dim;
    const FieldValue & vel;
    const FieldValue & vel_t;
    const FieldGradient & vel_x;
    const FieldValue & ffn;
};

class ResidualVeloc1 : public ResidualFunc {
public:
    explicit ResidualVeloc1(NSIncompressibleProblem * prob) :
        ResidualFunc(prob),
        n_comp(get_spatial_dimension()),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity")),
        press(get_field_value("pressure")),
        Re(prob->get_reynolds_number())
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        for (PetscInt comp = 0; comp < this->n_comp; ++comp) {
            for (PetscInt d = 0; d < this->dim; ++d) {
                f[comp * this->dim + d] = 1.0 / this->Re * this->vel_x(d, comp);
            }
            f[comp * this->dim + comp] -= this->press(0);
        }
    }

protected:
    const PetscInt & n_comp;
    const PetscInt & dim;
    const FieldGradient & vel_x;
    const FieldValue & press;
    const PetscScalar & Re;
};

class ResidualPress0 : public ResidualFunc {
public:
    explicit ResidualPress0(NSIncompressibleProblem * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        f[0] = 0.0;
        for (PetscInt d = 0; d < this->dim; ++d)
            f[0] += this->vel_x(d, d);
    }

protected:
    const PetscInt & dim;
    const FieldGradient & vel_x;
};

class ResidualPress1 : public ResidualFunc {
public:
    explicit ResidualPress1(NSIncompressibleProblem * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            f[d] = 0.0;
    }

protected:
    const PetscInt & dim;
};

class JacobianVV0 : public JacobianFunc {
public:
    explicit JacobianVV0(NSIncompressibleProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity")),
        u_t_shift(get_time_shift())
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        PetscInt nc_i = this->dim;
        PetscInt nc_j = this->dim;

        for (PetscInt d = 0; d < this->dim; ++d) {
            g[d * this->dim + d] = this->u_t_shift;
        }

        for (PetscInt fc = 0; fc < nc_i; ++fc) {
            for (PetscInt gc = 0; gc < nc_j; ++gc) {
                g[fc * nc_j + gc] += this->vel_x(gc, fc);
            }
        }
    }

protected:
    const PetscInt & dim;
    const FieldGradient & vel_x;
    const PetscReal & u_t_shift;
};

class JacobianVV1 : public JacobianFunc {
public:
    explicit JacobianVV1(NSIncompressibleProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension()),
        vel(get_field_value("velocity"))
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        PetscInt nc_i = this->dim;
        PetscInt nc_j = this->dim;

        for (PetscInt fc = 0; fc < nc_i; ++fc) {
            for (PetscInt gc = 0; gc < nc_j; ++gc) {
                for (PetscInt dg = 0; dg < this->dim; ++dg) {
                    // kronecker delta
                    if (fc == gc)
                        g[(fc * nc_j + gc) * this->dim + dg] += this->vel(dg);
                }
            }
        }
    }

protected:
    const PetscInt & dim;
    const FieldValue & vel;
};

class JacobianPV1 : public JacobianFunc {
public:
    explicit JacobianPV1(NSIncompressibleProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.0;
    }

protected:
    const PetscInt & dim;
};

class JacobianVP2 : public JacobianFunc {
public:
    explicit JacobianVP2(NSIncompressibleProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = -1.0;
    }

protected:
    const PetscInt & dim;
};

class JacobianVV3 : public JacobianFunc {
public:
    explicit JacobianVV3(NSIncompressibleProblem * prob) :
        JacobianFunc(prob),
        n_comp(get_spatial_dimension()),
        dim(get_spatial_dimension()),
        Re(prob->get_reynolds_number())
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        for (PetscInt comp_i = 0; comp_i < this->n_comp; ++comp_i) {
            for (PetscInt d = 0; d < this->dim; ++d) {
                g[((comp_i * this->n_comp + comp_i) * this->dim + d) * this->dim + d] =
                    1.0 / this->Re;
            }
        }
    }

protected:
    const PetscInt & n_comp;
    const PetscInt & dim;
    const PetscReal & Re;
};

} // namespace

Parameters
NSIncompressibleProblem::parameters()
{
    Parameters params = ImplicitFENonlinearProblem::parameters();
    params.add_required_param<PetscReal>("Re", "Reynolds number");
    return params;
}

NSIncompressibleProblem::NSIncompressibleProblem(const Parameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    Re(get_param<PetscReal>("Re"))
{
    _F_;
}

const PetscReal &
NSIncompressibleProblem::get_reynolds_number() const
{
    _F_;
    return this->Re;
}

void
NSIncompressibleProblem::set_up_fields()
{
    _F_;
    const char * comp_name[] = { "velocity_x", "velocity_y", "velocity_z" };

    PetscInt dim = this->get_dimension();
    set_fe(velocity_id, "velocity", dim, 2);
    for (unsigned int i = 0; i < dim; i++)
        set_field_component_name(velocity_id, i, comp_name[i]);
    set_fe(pressure_id, "pressure", 1, 1);

    set_aux_fe(ffn_aid, "ffn", dim, 2);
}

void
NSIncompressibleProblem::set_up_weak_form()
{
    _F_;
    add_residual_block(velocity_id, new ResidualVeloc0(this), new ResidualVeloc1(this));
    add_residual_block(pressure_id, new ResidualPress0(this), new ResidualPress1(this));

    add_jacobian_block(velocity_id,
                       velocity_id,
                       new JacobianVV0(this),
                       new JacobianVV1(this),
                       nullptr,
                       new JacobianVV3(this));
    add_jacobian_block(velocity_id, pressure_id, nullptr, nullptr, new JacobianVP2(this), nullptr);
    add_jacobian_block(pressure_id, velocity_id, nullptr, new JacobianPV1(this), nullptr, nullptr);
}

void
NSIncompressibleProblem::set_up_matrix_properties()
{
    _F_;
}

void
NSIncompressibleProblem::set_up_field_null_space(DM dm)
{
    _F_;
    // PetscObject pressure;
    // MatNullSpace nsp;
    // PETSC_CHECK(DMGetField(dm, pressure_id, nullptr, &pressure));
    // PETSC_CHECK(MatNullSpaceCreate(PetscObjectComm(pressure), PETSC_TRUE, 0, nullptr, &nsp));
    // PETSC_CHECK(PetscObjectCompose(pressure, "nullspace", (PetscObject) nsp));
    // PETSC_CHECK(MatNullSpaceDestroy(&nsp));
}

void
NSIncompressibleProblem::set_up_preconditioning()
{
    _F_;

    PC pc;
    PETSC_CHECK(KSPGetPC(this->ksp, &pc));

    PETSC_CHECK(PCSetType(pc, PCFIELDSPLIT));
    PETSC_CHECK(PCFieldSplitSetType(pc, PC_COMPOSITE_SCHUR));
    PETSC_CHECK(PCFieldSplitSetSchurFactType(pc, PC_FIELDSPLIT_SCHUR_FACT_FULL));
    PetscInt n_fields;
    char ** field_names;
    IS * is;
    PETSC_CHECK(DMCreateFieldIS(get_dm(), &n_fields, &field_names, &is));

    // attach null space to the pressure field
    MatNullSpace nsp;
    PETSC_CHECK(MatNullSpaceCreate(get_comm(), PETSC_TRUE, 0, nullptr, &nsp));
    PETSC_CHECK(PetscObjectCompose((PetscObject) is[1], "nullspace", (PetscObject) nsp));
    PETSC_CHECK(MatNullSpaceDestroy(&nsp));

    for (PetscInt i = 0; i < n_fields; i++)
        PETSC_CHECK(PCFieldSplitSetIS(pc, field_names[i], is[i]));
    for (PetscInt i = 0; i < n_fields; i++) {
        PetscFree(field_names[i]);
        PETSC_CHECK(ISDestroy(is + i));
    }
    PetscFree(field_names);
    PetscFree(is);
    PETSC_CHECK(PCSetOperators(pc, this->J, this->J));
    PETSC_CHECK(PCSetUp(pc));

    PetscInt n;
    KSP * sub_ksp;
    PETSC_CHECK(PCFieldSplitGetSubKSP(pc, &n, &sub_ksp));
    assert(n == 2);

    PC pc_vel;
    PETSC_CHECK(KSPGetPC(sub_ksp[velocity_id], &pc_vel));
    PETSC_CHECK(PCSetType(pc_vel, PCLU));

    PETSC_CHECK(KSPSetTolerances(sub_ksp[pressure_id],
                                 1.0e-10,
                                 PETSC_DEFAULT,
                                 PETSC_DEFAULT,
                                 PETSC_DEFAULT));
    PC pc_press;
    PETSC_CHECK(KSPGetPC(sub_ksp[pressure_id], &pc_press));
    PETSC_CHECK(PCSetType(pc_press, PCJACOBI));

    PetscFree(sub_ksp);
}
