#include "godzilla/DenseMatrix.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/CallStack.h"
#include "ImplicitFENonlinearProblem_test.h"
#include "godzilla/Types.h"

using namespace godzilla;

namespace {

class F0 : public ResidualFunc<WeakForm::F0> {
public:
    explicit F0(GTestImplicitFENonlinearProblem * prob) :
        ResidualFunc(prob),
        u_t(get_field_dot("u"))
    {
    }

    DynDenseVector<Scalar>
    evaluate() const override
    {
        DynDenseVector<Scalar> f;
        f(0) = this->u_t(0);
        return f;
    }

protected:
    const FieldValue & u_t;
};

class F1 : public ResidualFunc<WeakForm::F1> {
public:
    explicit F1(GTestImplicitFENonlinearProblem * prob) :
        ResidualFunc(prob),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

    DynDenseMatrix<Scalar>
    evaluate() const override
    {
        DynDenseMatrix<Scalar> f(this->dim, 1);
        for (Int d = 0; d < this->dim; ++d)
            f(d, 0) = this->u_x(d);
        return f;
    }

protected:
    const Int & dim;
    const FieldGradient & u_x;
};

class G0 : public JacobianFunc {
public:
    explicit G0(GTestImplicitFENonlinearProblem * prob) :
        JacobianFunc(prob),
        u_t_shift(get_time_shift())
    {
    }

    void
    evaluate(Scalar g[]) const override
    {
        g[0] = this->u_t_shift;
    }

protected:
    const Real & u_t_shift;
};

class G3 : public JacobianFunc {
public:
    explicit G3(GTestImplicitFENonlinearProblem * prob) :
        JacobianFunc(prob),
        dim(get_spatial_dimension())
    {
    }

    void
    evaluate(Scalar g[]) const override
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
    CALL_STACK_MSG();
    Int order = 1;
    set_field(this->iu, "u", 1, order);
}

void
GTestImplicitFENonlinearProblem::set_up_weak_form()
{
    add_residual_block<WeakForm::F0>(this->iu, new F0(this));
    add_residual_block<WeakForm::F1>(this->iu, new F1(this));
    add_jacobian_block<WeakForm::G0>(this->iu, this->iu, new G0(this));
    add_jacobian_block<WeakForm::G3>(this->iu, this->iu, new G3(this));
}
