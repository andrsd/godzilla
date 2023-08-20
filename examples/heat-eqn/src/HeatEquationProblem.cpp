#include "Godzilla.h"
#include "FunctionInterface.h"
#include "HeatEquationProblem.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "CallStack.h"

using namespace godzilla;

namespace {

class Residual0 : public ResidualFunc {
public:
    explicit Residual0(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        T_t(get_field_dot("temp")),
        q_ppp(get_field_value("q_ppp"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        f[0] = this->T_t(0) - this->q_ppp(0);
    }

protected:
    const FieldValue & T_t;
    const FieldValue & q_ppp;
};

class Residual1 : public ResidualFunc {
public:
    explicit Residual1(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        T_x(get_field_gradient("temp"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        _F_;
        for (PetscInt d = 0; d < this->dim; ++d)
            f[d] = this->T_x(d);
    }

protected:
    const PetscInt & dim;
    const FieldGradient & T_x;
};

class Jacobian0 : public JacobianFunc {
public:
    explicit Jacobian0(FEProblemInterface * fepi) : JacobianFunc(fepi), T_t_shift(get_time_shift())
    {
    }

    void
    evaluate(PetscScalar g[]) const override
    {
        _F_;
        g[0] = this->T_t_shift * 1.0;
    }

protected:
    const PetscReal & T_t_shift;
};

class Jacobian3 : public JacobianFunc {
public:
    explicit Jacobian3(FEProblemInterface * fepi) : JacobianFunc(fepi), dim(get_spatial_dimension())
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

} // namespace

REGISTER_OBJECT(HeatEquationProblem);

Parameters
HeatEquationProblem::parameters()
{
    Parameters params = ImplicitFENonlinearProblem::parameters();
    params.add_param<PetscInt>("p_order", 1, "Polynomial order of FE space");
    return params;
}

HeatEquationProblem::HeatEquationProblem(const Parameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    p_order(get_param<PetscInt>("p_order"))
{
    _F_;
}

void
HeatEquationProblem::set_up_fields()
{
    _F_;
    temp_id = add_fe("temp", 1, this->p_order);

    q_ppp_id = add_aux_fe("q_ppp", 1, 0);
    htc_aux_id = add_aux_fe("htc", 1, this->p_order);
    T_ambient_aux_id = add_aux_fe("T_ambient", 1, this->p_order);
}

void
HeatEquationProblem::set_up_weak_form()
{
    _F_;
    add_residual_block(temp_id, new Residual0(this), new Residual1(this));
    add_jacobian_block(temp_id,
                       temp_id,
                       new Jacobian0(this),
                       nullptr,
                       nullptr,
                       new Jacobian3(this));
}
