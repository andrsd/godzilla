#include "Godzilla.h"
#include "HeatEquationProblem.h"
#include "ConvectiveHeatFluxBC.h"
#include "BndResidualFunc.h"
#include "BndJacobianFunc.h"
#include "CallStack.h"

using namespace godzilla;

REGISTER_OBJECT(ConvectiveHeatFluxBC);

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
    evaluate(PetscScalar f[]) override
    {
        f[0] = htc[0] * (T[0] - T_infinity[0]);
    }

protected:
    const PetscScalar * T;
    const PetscScalar * htc;
    const PetscScalar * T_infinity;
};

class Jacobian0 : public BndJacobianFunc {
public:
    explicit Jacobian0(const NaturalBC * nbc) : BndJacobianFunc(nbc), htc(get_field_value("htc")) {}

    void
    evaluate(PetscScalar g[]) override
    {
        g[0] = htc[0];
    }

protected:
    const PetscScalar * htc;
};

} // namespace

Parameters
ConvectiveHeatFluxBC::parameters()
{
    Parameters params = NaturalBC::parameters();
    return params;
}

ConvectiveHeatFluxBC::ConvectiveHeatFluxBC(const Parameters & params) : NaturalBC(params)
{
    _F_;
}

PetscInt
ConvectiveHeatFluxBC::get_num_components() const
{
    return 1;
}

std::vector<PetscInt>
ConvectiveHeatFluxBC::get_components() const
{
    std::vector<PetscInt> comps = { 0 };
    return comps;
}

void
ConvectiveHeatFluxBC::set_up_weak_form()
{
    _F_;
    set_residual_block(new Residual0(this), nullptr);
    set_jacobian_block(get_field_id(), new Jacobian0(this), nullptr, nullptr, nullptr);
}
