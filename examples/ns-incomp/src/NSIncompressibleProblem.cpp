#include "godzilla/Godzilla.h"
#include "godzilla/FunctionInterface.h"
#include "NSIncompressibleProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/CallStack.h"
#include "godzilla/PCFieldSplit.h"
#include "godzilla/PCJacobi.h"
#include "godzilla/PCFactor.h"
#include "godzilla/PCComposite.h"
#include <cassert>
#include "petscsys.h"

using namespace godzilla;

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
        CALL_STACK_MSG();
        for (PetscInt c = 0; c < this->n_comp; ++c) {
            f[c] = this->vel_t(c);

            for (PetscInt d = 0; d < this->dim; ++d)
                f[c] += this->vel(d) * this->vel_x(c * this->dim + d);

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
        CALL_STACK_MSG();
        for (PetscInt comp = 0; comp < this->n_comp; ++comp) {
            for (PetscInt d = 0; d < this->dim; ++d) {
                f[comp * this->dim + d] = 1.0 / this->Re * this->vel_x(comp * this->dim + d);
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
        CALL_STACK_MSG();
        f[0] = 0.0;
        for (PetscInt d = 0; d < this->dim; ++d)
            f[0] += this->vel_x(d * this->dim + d);
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
        CALL_STACK_MSG();
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
        CALL_STACK_MSG();
        PetscInt nc_i = this->dim;
        PetscInt nc_j = this->dim;

        for (PetscInt d = 0; d < this->dim; ++d) {
            g[d * this->dim + d] = this->u_t_shift;
        }

        for (PetscInt fc = 0; fc < nc_i; ++fc) {
            for (PetscInt gc = 0; gc < nc_j; ++gc) {
                g[fc * nc_j + gc] += this->vel_x(fc * nc_j + gc);
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
        CALL_STACK_MSG();
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
        CALL_STACK_MSG();
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
        CALL_STACK_MSG();
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
        CALL_STACK_MSG();
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
    CALL_STACK_MSG();
}

const PetscReal &
NSIncompressibleProblem::get_reynolds_number() const
{
    CALL_STACK_MSG();
    return this->Re;
}

void
NSIncompressibleProblem::set_up_fields()
{
    CALL_STACK_MSG();
    const char * comp_name[] = { "velocity_x", "velocity_y", "velocity_z" };

    PetscInt dim = this->get_dimension();
    velocity_id = add_field("velocity", dim, 2);
    for (unsigned int i = 0; i < dim; i++)
        set_field_component_name(velocity_id, i, comp_name[i]);
    pressure_id = add_field("pressure", 1, 1);

    ffn_aid = add_aux_fe("ffn", dim, 2);
}

void
NSIncompressibleProblem::set_up_weak_form()
{
    CALL_STACK_MSG();
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
    CALL_STACK_MSG();
}

void
NSIncompressibleProblem::set_up_field_null_space(DM dm)
{
    CALL_STACK_MSG();
    // PetscObject pressure;
    // MatNullSpace nsp;
    // PETSC_CHECK(DMGetField(dm, pressure_id, nullptr, &pressure));
    // PETSC_CHECK(MatNullSpaceCreate(PetscObjectComm(pressure), PETSC_TRUE, 0, nullptr, &nsp));
    // PETSC_CHECK(PetscObjectCompose(pressure, "nullspace", (PetscObject) nsp));
    // PETSC_CHECK(MatNullSpaceDestroy(&nsp));
}

Preconditioner
NSIncompressibleProblem::create_preconditioner(PC pc)
{
    CALL_STACK_MSG();

    PCFieldSplit fsplit(pc);
    fsplit.set_type(PCFieldSplit::SCHUR);
    fsplit.set_schur_fact_type(PCFieldSplit::SCHUR_FACT_FULL);

    auto fdecomp = create_field_decomposition();
    // attach null space to the pressure field
    MatNullSpace nsp;
    PETSC_CHECK(MatNullSpaceCreate(get_comm(), PETSC_TRUE, 0, nullptr, &nsp));
    PETSC_CHECK(
        PetscObjectCompose((PetscObject) (IS) fdecomp.is[1], "nullspace", (PetscObject) nsp));
    PETSC_CHECK(MatNullSpaceDestroy(&nsp));
    // TODO: fdecomp.is[1].attach_null_space("nullspace");
    for (PetscInt i = 0; i < fdecomp.get_num_fields(); i++)
        fsplit.set_is(fdecomp.field_name[i], fdecomp.is[i]);
    fdecomp.destroy();

    auto J = get_jacobian();
    fsplit.set_operators(J, J);
    fsplit.set_up();

    auto sub_ksp = fsplit.get_sub_ksp();

    auto precond_vel = sub_ksp[velocity_id].set_pc_type<PCFactor>();
    precond_vel.set_type(PCFactor::LU);

    auto & ksp_press = sub_ksp[pressure_id];
    ksp_press.set_tolerances(1.0e-10, PETSC_DEFAULT, PETSC_DEFAULT, PETSC_DEFAULT);
    auto precond_press = ksp_press.set_pc_type<PCJacobi>();

    return fsplit;
}
