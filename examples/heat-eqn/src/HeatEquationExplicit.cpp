#include "Godzilla.h"
#include "FunctionInterface.h"
#include "HeatEquationExplicit.h"
#include "ResidualFunc.h"
#include "CallStack.h"

using namespace godzilla;

namespace {

class Residual0 : public ResidualFunc {
public:
    explicit Residual0(const FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        ffn(get_field_value("forcing_fn"))
    {
    }

    void
    evaluate(PetscScalar f[]) override
    {
        f[0] = -this->ffn[0];
    }

protected:
    const FieldValue & ffn;
};

class Residual1 : public ResidualFunc {
public:
    explicit Residual1(const FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        T_x(get_field_gradient("temp"))
    {
    }

    void
    evaluate(PetscScalar f[]) override
    {
        for (PetscInt d = 0; d < this->dim; ++d)
            f[d] = -this->T_x[d];
    }

protected:
    const PetscInt & dim;
    const FieldGradient & T_x;
};

} // namespace

REGISTER_OBJECT(HeatEquationExplicit);

Parameters
HeatEquationExplicit::parameters()
{
    Parameters params = ExplicitFELinearProblem::parameters();
    params.add_param<PetscInt>("order", 1, "Polynomial order of the FE space");
    return params;
}

HeatEquationExplicit::HeatEquationExplicit(const Parameters & parameters) :
    ExplicitFELinearProblem(parameters),
    order(get_param<PetscInt>("order"))
{
    _F_;
}

void
HeatEquationExplicit::set_up_fields()
{
    _F_;
    add_fe(temp_id, "temp", 1, this->order);
    add_aux_fe(ffn_aux_id, "forcing_fn", 1, this->order);
}

void
HeatEquationExplicit::set_up_weak_form()
{
    _F_;
    set_residual_block(temp_id, new Residual0(this), new Residual1(this));
}
