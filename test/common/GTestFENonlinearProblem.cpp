#include "GTestFENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Types.h"

namespace {

class F0 : public ResidualFunc {
public:
    explicit F0(GTestFENonlinearProblem * prob) : ResidualFunc(prob) {}

    void
    evaluate(Scalar f[]) const override
    {
        f[0] = 2.0;
    }
};

class F1 : public ResidualFunc {
public:
    explicit F1(GTestFENonlinearProblem * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) const override
    {
        for (Int d = 0; d < this->dim; ++d)
            f[d] = this->u_x(d);
    }

protected:
    const Dimension & dim;
    const FieldGradient & u_x;
};

class G3 : public JacobianFunc {
public:
    explicit G3(GTestFENonlinearProblem * prob) : JacobianFunc(prob), dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.;
    }

protected:
    const Dimension & dim;
};

} // namespace

GTestFENonlinearProblem::GTestFENonlinearProblem(const Parameters & pars) :
    FENonlinearProblem(pars),
    iu(0)
{
}

void
GTestFENonlinearProblem::set_up_initial_guess()
{
    FENonlinearProblem::set_up_initial_guess();
}

PetscDS
GTestFENonlinearProblem::get_ds()
{
    return FENonlinearProblem::get_ds();
}

void
GTestFENonlinearProblem::set_up_fields()
{
    set_field(this->iu, "u", 1, Order(1));
}

void
GTestFENonlinearProblem::set_up_weak_form()
{
    add_residual_block(this->iu, new F0(this), new F1(this));
    add_jacobian_block(this->iu, this->iu, nullptr, nullptr, nullptr, new G3(this));
}
