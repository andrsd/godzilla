#include "godzilla/DenseMatrix.h"
#include "BurgersEquation.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/CallStack.h"
#include "godzilla/Types.h"
#include "godzilla/WeakForm.h"

using namespace godzilla;

namespace {

class F1 : public ResidualFunc<WeakForm::F1> {
public:
    explicit F1(BurgersEquation * prob) :
        ResidualFunc(prob),
        visc(prob->get_viscosity()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u"))
    {
    }

    DynDenseMatrix<Scalar>
    evaluate() const override
    {
        CALL_STACK_MSG();
        DynDenseMatrix<Scalar> f(1, 1);
        f(0, 0) = -this->visc * this->u_x(0) + 0.5 * this->u(0) * this->u(0);
        return f;
    }

protected:
    const Real & visc;
    const FieldValue & u;
    const FieldGradient & u_x;
};

} // namespace

///

Parameters
BurgersEquation::parameters()
{
    Parameters params = ExplicitFELinearProblem::parameters();
    params.add_param<Real>("viscosity", "Viscosity");
    return params;
}

BurgersEquation::BurgersEquation(const Parameters & parameters) :
    ExplicitFELinearProblem(parameters),
    viscosity(get_param<Real>("viscosity"))
{
    CALL_STACK_MSG();
}

void
BurgersEquation::create()
{
    CALL_STACK_MSG();
    ExplicitFELinearProblem::create();
    create_mass_matrix();
}

const Real &
BurgersEquation::get_viscosity() const
{
    CALL_STACK_MSG();
    return this->viscosity;
}

void
BurgersEquation::set_up_fields()
{
    CALL_STACK_MSG();
    u_id = add_field("u", 1, 1);
}

void
BurgersEquation::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block<WeakForm::F1>(u_id, new F1(this));
}
