#include "gmock/gmock.h"
#include "GodzillaConfig.h"
#include "ResidualFunc.h"
#include "JacobianFunc.h"
#include "CallStack.h"
#include "ImplicitFENonlinearProblem_test.h"

using namespace godzilla;

REGISTER_OBJECT(GTestImplicitFENonlinearProblem);

namespace {

class F0 : public ResidualFunc {
public:
    explicit F0(const GTestImplicitFENonlinearProblem * prob) :
        ResidualFunc(prob),
        u_t(get_field_dot("u"))
    {
    }

    void
    evaluate(Scalar f[]) override
    {
        f[0] = this->u_t[0];
    }

protected:
    const FieldValue & u_t;
};

class F1 : public ResidualFunc {
public:
    explicit F1(const GTestImplicitFENonlinearProblem * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    void
    evaluate(Scalar f[]) override
    {
        for (Int d = 0; d < this->dim; ++d)
            f[d] = this->u_x[d];
    }

protected:
    const Int & dim;
    const FieldGradient & u_x;
};

class G0 : public JacobianFunc {
public:
    explicit G0(const GTestImplicitFENonlinearProblem * prob) :
        JacobianFunc(prob),
        u_t_shift(get_time_shift())
    {
    }

    void
    evaluate(Scalar g[]) override
    {
        g[0] = this->u_t_shift;
    }

protected:
    const Real & u_t_shift;
};

class G3 : public JacobianFunc {
public:
    explicit G3(const GTestImplicitFENonlinearProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) override
    {
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.;
    }

protected:
    const Int & dim;
};

} // namespace

GTestImplicitFENonlinearProblem::GTestImplicitFENonlinearProblem(const Parameters & params) :
    ImplicitFENonlinearProblem(params),
    iu(0)
{
}

void
GTestImplicitFENonlinearProblem::set_up_initial_guess()
{
    ImplicitFENonlinearProblem::set_up_initial_guess();
}

void
GTestImplicitFENonlinearProblem::set_up_fields()
{
    _F_;
    Int order = 1;
    set_fe(this->iu, "u", 1, order);
}

void
GTestImplicitFENonlinearProblem::set_up_weak_form()
{
    set_residual_block(this->iu, new F0(this), new F1(this));
    set_jacobian_block(this->iu, this->iu, new G0(this), nullptr, nullptr, new G3(this));
}
