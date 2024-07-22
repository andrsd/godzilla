#include "godzilla/DenseMatrix.h"
#include "HeatEquationProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/WeakForm.h"

using namespace godzilla;

namespace {

class Residual0 : public ResidualFunc<WeakForm::F0> {
public:
    explicit Residual0(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        T_t(get_field_dot("temp")),
        q_ppp(get_field_value("q_ppp"))
    {
    }

    DynDenseVector<Scalar>
    evaluate() const override
    {
        CALL_STACK_MSG();
        DynDenseVector<Scalar> f(1);
        f(0) = this->T_t(0) - this->q_ppp(0);
        return f;
    }

protected:
    const FieldValue & T_t;
    const FieldValue & q_ppp;
};

class Residual1 : public ResidualFunc<WeakForm::F1> {
public:
    explicit Residual1(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        T_x(get_field_gradient("temp"))
    {
    }

    DynDenseMatrix<Scalar>
    evaluate() const override
    {
        CALL_STACK_MSG();
        DynDenseMatrix<Scalar> f(this->dim, 1);
        for (Int d = 0; d < this->dim; ++d)
            f(d, 0) = this->T_x(d);
        return f;
    }

protected:
    const Int & dim;
    const FieldGradient & T_x;
};

class Jacobian0 : public JacobianFunc {
public:
    explicit Jacobian0(FEProblemInterface * fepi) : JacobianFunc(fepi), T_t_shift(get_time_shift())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        g[0] = this->T_t_shift * 1.0;
    }

protected:
    const Real & T_t_shift;
};

class Jacobian3 : public JacobianFunc {
public:
    explicit Jacobian3(FEProblemInterface * fepi) : JacobianFunc(fepi), dim(get_spatial_dimension())
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
    const Int & dim;
};

} // namespace

Parameters
HeatEquationProblem::parameters()
{
    Parameters params = ImplicitFENonlinearProblem::parameters();
    params.add_param<Int>("p_order", 1, "Polynomial order of FE space");
    return params;
}

HeatEquationProblem::HeatEquationProblem(const Parameters & parameters) :
    ImplicitFENonlinearProblem(parameters),
    p_order(get_param<Int>("p_order"))
{
    CALL_STACK_MSG();
}

void
HeatEquationProblem::set_up_fields()
{
    CALL_STACK_MSG();
    temp_id = add_field("temp", 1, this->p_order);

    q_ppp_id = add_aux_field("q_ppp", 1, 0);
    htc_aux_id = add_aux_field("htc", 1, this->p_order);
    T_ambient_aux_id = add_aux_field("T_ambient", 1, this->p_order);
}

void
HeatEquationProblem::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block<WeakForm::F0>(temp_id, new Residual0(this));
    add_residual_block<WeakForm::F1>(temp_id, new Residual1(this));

    add_jacobian_block<WeakForm::G0>(temp_id, temp_id, new Jacobian0(this));
    add_jacobian_block<WeakForm::G3>(temp_id, temp_id, new Jacobian3(this));
}
