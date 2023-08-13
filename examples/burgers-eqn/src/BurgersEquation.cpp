#include "Godzilla.h"
#include "FunctionInterface.h"
#include "BurgersEquation.h"
#include "ResidualFunc.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(BurgersEquation);

namespace {

class F1 : public ResidualFunc {
public:
    explicit F1(BurgersEquation * prob) :
        ResidualFunc(prob),
        visc(prob->get_viscosity()),
        u(get_field_value("u")),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(PetscScalar f[]) const override
    {
        f[0] = -this->visc * this->u_x(0) + 0.5 * this->u(0) * this->u(0);
    }

protected:
    const PetscReal & visc;
    const FieldValue & u;
    const FieldGradient & u_x;
};

} // namespace

///

Parameters
BurgersEquation::parameters()
{
    Parameters params = ExplicitFELinearProblem::parameters();
    params.add_param<PetscReal>("viscosity", "Viscosity");
    return params;
}

BurgersEquation::BurgersEquation(const Parameters & parameters) :
    ExplicitFELinearProblem(parameters),
    viscosity(get_param<PetscReal>("viscosity"))
{
    _F_;
}

void
BurgersEquation::create()
{
    _F_;
    ExplicitFELinearProblem::create();
    create_mass_matrix();
}

const PetscReal &
BurgersEquation::get_viscosity() const
{
    _F_;
    return this->viscosity;
}

void
BurgersEquation::set_up_fields()
{
    _F_;
    set_fe(u_id, "u", 1, 1);
}

void
BurgersEquation::set_up_weak_form()
{
    _F_;
    add_residual_block(u_id, nullptr, new F1(this));
}
