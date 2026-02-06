#include "NSIncompressibleProblem.h"
#include "godzilla/PCFieldSplit.h"
#include "godzilla/Preconditioner.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/CallStack.h"
#include "godzilla/PCJacobi.h"
#include "godzilla/PCFactor.h"
#include "godzilla/PCComposite.h"
#include <cassert>
#include "godzilla/Types.h"
#include "petscsys.h"

using namespace godzilla;

namespace {

class ResidualVeloc0 : public ResidualFunc {
public:
    explicit ResidualVeloc0(Ref<NSIncompressibleProblem> prob) :
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
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        for (Int c = 0; c < this->n_comp; ++c) {
            f[c] = this->vel_t(c);

            for (Int d = 0; d < this->dim; ++d)
                f[c] += this->vel(d) * this->vel_x(c * this->dim + d);

            f[c] -= this->ffn(c);
        }
    }

protected:
    const Int n_comp;
    const Dimension & dim;
    const FieldValue & vel;
    const FieldValue & vel_t;
    const FieldGradient & vel_x;
    const FieldValue & ffn;
};

class ResidualVeloc1 : public ResidualFunc {
public:
    explicit ResidualVeloc1(Ref<NSIncompressibleProblem> prob) :
        ResidualFunc(prob),
        n_comp(get_spatial_dimension()),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity")),
        press(get_field_value("pressure")),
        Re(prob->get_reynolds_number())
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        for (Int comp = 0; comp < this->n_comp; ++comp) {
            for (Int d = 0; d < this->dim; ++d) {
                f[comp * this->dim + d] = 1.0 / this->Re * this->vel_x(comp * this->dim + d);
            }
            f[comp * this->dim + comp] -= this->press(0);
        }
    }

protected:
    const Int n_comp;
    const Dimension & dim;
    const FieldGradient & vel_x;
    const FieldValue & press;
    const Scalar & Re;
};

class ResidualPress0 : public ResidualFunc {
public:
    explicit ResidualPress0(Ref<NSIncompressibleProblem> prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        f[0] = 0.0;
        for (Int d = 0; d < this->dim; ++d)
            f[0] += this->vel_x(d * this->dim + d);
    }

protected:
    const Dimension & dim;
    const FieldGradient & vel_x;
};

class ResidualPress1 : public ResidualFunc {
public:
    explicit ResidualPress1(Ref<NSIncompressibleProblem> prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            f[d] = 0.0;
    }

protected:
    const Dimension & dim;
};

class JacobianVV0 : public JacobianFunc {
public:
    explicit JacobianVV0(Ref<NSIncompressibleProblem> prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension()),
        vel_x(get_field_gradient("velocity")),
        u_t_shift(get_time_shift())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        Int nc_i = this->dim;
        Int nc_j = this->dim;

        for (Int d = 0; d < this->dim; ++d) {
            g[d * this->dim + d] = this->u_t_shift;
        }

        for (Int fc = 0; fc < nc_i; ++fc) {
            for (Int gc = 0; gc < nc_j; ++gc) {
                g[fc * nc_j + gc] += this->vel_x(fc * nc_j + gc);
            }
        }
    }

protected:
    const Dimension & dim;
    const FieldGradient & vel_x;
    const Real & u_t_shift;
};

class JacobianVV1 : public JacobianFunc {
public:
    explicit JacobianVV1(Ref<NSIncompressibleProblem> prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension()),
        vel(get_field_value("velocity"))
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        Int nc_i = this->dim;
        Int nc_j = this->dim;

        for (Int fc = 0; fc < nc_i; ++fc) {
            for (Int gc = 0; gc < nc_j; ++gc) {
                for (Int dg = 0; dg < this->dim; ++dg) {
                    // kronecker delta
                    if (fc == gc)
                        g[(fc * nc_j + gc) * this->dim + dg] += this->vel(dg);
                }
            }
        }
    }

protected:
    const Dimension & dim;
    const FieldValue & vel;
};

class JacobianPV1 : public JacobianFunc {
public:
    explicit JacobianPV1(Ref<NSIncompressibleProblem> prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.0;
    }

protected:
    const Dimension & dim;
};

class JacobianVP2 : public JacobianFunc {
public:
    explicit JacobianVP2(Ref<NSIncompressibleProblem> prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = -1.0;
    }

protected:
    const Dimension & dim;
};

class JacobianVV3 : public JacobianFunc {
public:
    explicit JacobianVV3(Ref<NSIncompressibleProblem> prob) :
        JacobianFunc(prob),
        n_comp(get_spatial_dimension()),
        dim(get_spatial_dimension()),
        Re(prob->get_reynolds_number())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        for (Int comp_i = 0; comp_i < this->n_comp; ++comp_i) {
            for (Int d = 0; d < this->dim; ++d) {
                g[((comp_i * this->n_comp + comp_i) * this->dim + d) * this->dim + d] =
                    1.0 / this->Re;
            }
        }
    }

protected:
    const Int n_comp;
    const Dimension & dim;
    const Real & Re;
};

} // namespace

Parameters
NSIncompressibleProblem::parameters()
{
    auto params = ImplicitFENonlinearProblem::parameters();
    params.add_required_param<Real>("Re", "Reynolds number");
    return params;
}

NSIncompressibleProblem::NSIncompressibleProblem(const Parameters & pars) :
    ImplicitFENonlinearProblem(pars),
    velocity_id(FieldID::INVALID),
    pressure_id(FieldID::INVALID),
    ffn_aid(FieldID::INVALID),
    Re(pars.get<Real>("Re"))
{
    CALL_STACK_MSG();
}

const Real &
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

    Int dim = this->get_dimension();
    this->velocity_id = add_field("velocity", dim, Order(2));
    for (unsigned int i = 0; i < dim; ++i)
        set_field_component_name(this->velocity_id, i, comp_name[i]);
    this->pressure_id = add_field("pressure", 1, Order(1));

    this->ffn_aid = add_aux_field("ffn", dim, Order(2));
}

void
NSIncompressibleProblem::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block(this->velocity_id,
                       new ResidualVeloc0(ref(*this)),
                       new ResidualVeloc1(ref(*this)));
    add_residual_block(this->pressure_id,
                       new ResidualPress0(ref(*this)),
                       new ResidualPress1(ref(*this)));

    add_jacobian_block(this->velocity_id,
                       this->velocity_id,
                       new JacobianVV0(ref(*this)),
                       new JacobianVV1(ref(*this)),
                       nullptr,
                       new JacobianVV3(ref(*this)));
    add_jacobian_block(this->velocity_id,
                       this->pressure_id,
                       nullptr,
                       nullptr,
                       new JacobianVP2(ref(*this)),
                       nullptr);
    add_jacobian_block(this->pressure_id,
                       this->velocity_id,
                       nullptr,
                       new JacobianPV1(ref(*this)),
                       nullptr,
                       nullptr);
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

    this->fsplit = PCFieldSplit(pc);
    this->fsplit.inc_reference();
    this->fsplit.set_type(PCFieldSplit::SCHUR);
    this->fsplit.set_schur_fact_type(PCFieldSplit::SCHUR_FACT_FULL);

    auto fdecomp = create_field_decomposition();
    // attach null space to the pressure field
    MatNullSpace nsp;
    PETSC_CHECK(MatNullSpaceCreate(get_comm(), PETSC_TRUE, 0, nullptr, &nsp));
    PETSC_CHECK(
        PetscObjectCompose((PetscObject) (IS) fdecomp.is[1], "nullspace", (PetscObject) nsp));
    PETSC_CHECK(MatNullSpaceDestroy(&nsp));
    // TODO: fdecomp.is[1].attach_null_space("nullspace");
    for (Int i = 0; i < fdecomp.get_num_fields(); ++i)
        fsplit.set_is(fdecomp.field_name[i], fdecomp.is[i]);
    fdecomp.destroy();

    auto J = get_jacobian();
    this->fsplit.set_operators(J, J);
    this->fsplit.set_up();

    auto sub_ksp = this->fsplit.get_sub_ksp();

    auto precond_vel = sub_ksp[this->velocity_id.value()].set_pc_type<PCFactor>();
    precond_vel.set_type(PCFactor::LU);

    auto & ksp_press = sub_ksp[this->pressure_id.value()];
    ksp_press.set_tolerances(1.0e-10, PETSC_DEFAULT, PETSC_DEFAULT, PETSC_DEFAULT);
    auto precond_press = ksp_press.set_pc_type<PCJacobi>();

    return this->fsplit;
}
