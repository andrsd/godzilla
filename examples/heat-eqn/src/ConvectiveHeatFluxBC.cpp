#include "godzilla/Godzilla.h"
#include "ConvectiveHeatFluxBC.h"
#include "godzilla/BndResidualFunc.h"
#include "godzilla/BndJacobianFunc.h"
#include "godzilla/CallStack.h"

using namespace godzilla;

namespace {

class Residual0 : public BndResidualFunc {
public:
    explicit Residual0(const NaturalBC * nbc) :
        BndResidualFunc(nbc),
        T(get_field_value("temp")),
        htc(get_field_value("htc")),
        T_infinity(get_field_value("T_ambient"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        CALL_STACK_MSG();
        f[0] = htc(0) * (T(0) - T_infinity(0));
    }

protected:
    const FieldValue & T;
    const FieldValue & htc;
    const FieldValue & T_infinity;
};

class Jacobian0 : public BndJacobianFunc {
public:
    explicit Jacobian0(const NaturalBC * nbc) : BndJacobianFunc(nbc), htc(get_field_value("htc")) {}

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        g[0] = htc(0);
    }

protected:
    const FieldValue & htc;
};

} // namespace

Parameters
ConvectiveHeatFluxBC::parameters()
{
    Parameters params = NaturalBC::parameters();
    return params;
}

ConvectiveHeatFluxBC::ConvectiveHeatFluxBC(const Parameters & params) :
    NaturalBC(params),
    components({ 0 })
{
    CALL_STACK_MSG();
}

const std::vector<Int> &
ConvectiveHeatFluxBC::get_components() const
{
    return this->components;
}

void
ConvectiveHeatFluxBC::set_up_weak_form()
{
    CALL_STACK_MSG();
    add_residual_block<WeakForm::BND_F0>(new Residual0(this));
    add_jacobian_block<WeakForm::BND_G0>(get_field_id(), new Jacobian0(this));
}
