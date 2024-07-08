#include "godzilla/Godzilla.h"
#include "godzilla/FunctionInterface.h"
#include "HeatEquationExplicit.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/CallStack.h"

using namespace godzilla;

namespace {

class Residual0 : public ResidualFunc {
public:
    explicit Residual0(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        ffn(get_field_value("forcing_fn"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        f[0] = -this->ffn(0);
    }

protected:
    const FieldValue & ffn;
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
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            f[d] = -this->T_x(d);
    }

protected:
    const Int & dim;
    const FieldGradient & T_x;
};

} // namespace

Parameters
HeatEquationExplicit::parameters()
{
    Parameters params = ExplicitFELinearProblem::parameters();
    params.add_param<Int>("order", 1, "Polynomial order of the FE space");
    return params;
}

HeatEquationExplicit::HeatEquationExplicit(const Parameters & parameters) :
    ExplicitFELinearProblem(parameters),
    order(get_param<Int>("order"))
{
    CALL_STACK_MSG();
}

void
HeatEquationExplicit::create()
{
    CALL_STACK_MSG();
    ExplicitFELinearProblem::create();
    create_mass_matrix();
}

void
HeatEquationExplicit::set_up_fields()
{
    CALL_STACK_MSG();
    temp_id = add_field("temp", 1, this->order);
    ffn_aux_id = add_aux_field("forcing_fn", 1, this->order);
}

void
HeatEquationExplicit::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block(temp_id, new Residual0(this), new Residual1(this));
}
