#include "GTestFENonlinearProblem.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/WeakForm.h"

namespace {

class F0 : public ResidualFunc<WeakForm::F0> {
public:
    explicit F0(GTestFENonlinearProblem * prob) : ResidualFunc(prob) {}

    DynDenseVector<Scalar>
    evaluate() const override
    {
        DynDenseVector<Scalar> f(1);
        f(0) = 2.0;
        return f;
    }
};

class F1 : public ResidualFunc<WeakForm::F1> {
public:
    explicit F1(GTestFENonlinearProblem * prob) :
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
    const Int & dim;
};

} // namespace

GTestFENonlinearProblem::GTestFENonlinearProblem(const Parameters & params) :
    FENonlinearProblem(params),
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
    Int order = 1;
    set_field(this->iu, "u", 1, order);
}

void
GTestFENonlinearProblem::set_up_weak_form()
{
    add_residual_block<WeakForm::F0>(this->iu, new F0(this));
    add_residual_block<WeakForm::F1>(this->iu, new F1(this));
    add_jacobian_block<WeakForm::G3>(this->iu, this->iu, new G3(this));
}
