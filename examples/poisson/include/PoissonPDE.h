#pragma once

#include "godzilla/DenseMatrix.h"
#include "godzilla/ResidualFunc.h"
#include "godzilla/JacobianFunc.h"
#include "godzilla/Types.h"

using namespace godzilla;

class Residual0 : public ResidualFunc<WeakForm::F0> {
public:
    Residual0(FEProblemInterface * fepi) : ResidualFunc(fepi), ffn(get_field_value("forcing_fn")) {}

private:
    DynDenseVector<Scalar>
    evaluate() const override
    {
        CALL_STACK_MSG();
        DynDenseVector<Scalar> f(1);
        f(0) = -this->ffn(0);
        return f;
    }

    const FieldValue & ffn;
};

class Residual1 : public ResidualFunc<WeakForm::F1> {
public:
    Residual1(FEProblemInterface * fepi) :
        ResidualFunc(fepi),
        dim(get_spatial_dimension()),
        u_x(get_field_gradient("u"))
    {
    }

private:
    DynDenseMatrix<Scalar>
    evaluate() const override
    {
        CALL_STACK_MSG();
        DynDenseMatrix<Scalar> f(this->dim, 1);
        for (Int d = 0; d < this->dim; ++d)
            f(d, 0) = this->u_x(d);
        return f;
    }

    const Int & dim;
    const FieldGradient & u_x;
};

class Jacobian3 : public JacobianFunc {
public:
    Jacobian3(FEProblemInterface * fepi) : JacobianFunc(fepi), dim(get_spatial_dimension()) {}

    void
    evaluate(Scalar g[]) const override
    {
        CALL_STACK_MSG();
        for (Int d = 0; d < this->dim; ++d)
            g[d * this->dim + d] = 1.0;
    }

protected:
    const Int & dim;
};
